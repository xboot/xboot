/*
 * realview-mmc.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <xboot.h>
#include <sizes.h>
#include <types.h>
#include <stddef.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <time/delay.h>
#include <mmc/mmc.h>
#include <mmc/mmc_host.h>
#include <realview/reg-mmc.h>

/*
 * send command flags
 */
#define REALVIEW_MCI_RSP_NONE			(0 << 0)
#define REALVIEW_MCI_RSP_PRESENT		(1 << 0)
#define REALVIEW_MCI_RSP_136BIT			(1 << 1)
#define REALVIEW_MCI_RSP_CRC			(1 << 2)

static bool_t mmc_send_cmd(u32_t cmd, u32_t arg, u32_t * resp, u32_t flags)
{
	u32_t status;
	bool_t ret = TRUE;

	if(readl(REALVIEW_MCI_COMMAND) & REALVIEW_MCI_CMD_ENABLE)
	{
		writel(REALVIEW_MCI_COMMAND, 0x0);
	}

	cmd = (cmd & 0x3f) | REALVIEW_MCI_CMD_ENABLE;

	if(flags & REALVIEW_MCI_RSP_PRESENT)
	{
		if(flags & REALVIEW_MCI_RSP_136BIT)
			cmd |= REALVIEW_MCI_CMD_LONGRSP;
		cmd |= REALVIEW_MCI_CMD_RESPONSE;
	}

	writel(REALVIEW_MCI_ARGUMENT, arg);
	writel(REALVIEW_MCI_COMMAND, cmd);

	/*
	 * wait for a while
	 */
	do {
		status = readl(REALVIEW_MCI_STATUS);
	} while(!(status & (REALVIEW_MCI_STAT_CMD_SENT | REALVIEW_MCI_STAT_CMD_RESP_END |
			REALVIEW_MCI_STAT_CMD_TIME_OUT | REALVIEW_MCI_STAT_CMD_CRC_FAIL)));

	if(flags & REALVIEW_MCI_RSP_PRESENT)
	{
		resp[0] = readl(REALVIEW_MCI_RESP0);

		if(flags & REALVIEW_MCI_RSP_136BIT)
		{
			resp[1] = readl(REALVIEW_MCI_RESP1);
			resp[2] = readl(REALVIEW_MCI_RESP2);
			resp[3] = readl(REALVIEW_MCI_RESP3);
		}
	}

	if(status & REALVIEW_MCI_STAT_CMD_TIME_OUT)
		ret = FALSE;
	else if ((status & REALVIEW_MCI_STAT_CMD_CRC_FAIL) && (flags & REALVIEW_MCI_RSP_CRC))
		ret = FALSE;

	writel(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_CMD_SENT | REALVIEW_MCI_CLR_CMD_RESP_END |
			REALVIEW_MCI_CLR_CMD_TIMEOUT | REALVIEW_MCI_CLR_CMD_CRC_FAIL));

	return ret;
}

static bool_t mmc_send_acmd(u32_t cmd, u32_t arg, u32_t * resp, u32_t flags)
{
	u32_t aresp;
	bool_t ret = TRUE;

	ret = mmc_send_cmd(MMC_APP_CMD, 0, &aresp, REALVIEW_MCI_RSP_PRESENT);
	if(!ret)
		return ret;

	if((aresp & ((1 << 22) | (1 << 5))) != (1 << 5))
		return FALSE;

	return mmc_send_cmd(cmd, arg, resp, flags);
}

static bool_t mmc_idle_cards(void)
{
	bool_t ret = TRUE;

	/*
	 * reset all cards
	 */
	ret = mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, REALVIEW_MCI_RSP_NONE);
	if(ret)
		return ret;

	/*
	 * wait a moment
	 */
	udelay(1000);

	return mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, 0);
}

static bool_t mmc_send_if_cond(struct mmc_card_info * info)
{
	u32_t resp[4];

	if(!mmc_idle_cards())
		return FALSE;

	if(mmc_send_cmd(SD_SEND_IF_COND, MMC_VDD_33_34, &resp[0], REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC))
	{
		if((resp[0] & 0xff) == 0xaa)
		{
			info->type = MMC_CARD_TYPE_SD20;
			return TRUE;
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

static bool_t sd_send_op_cond(struct mmc_card_info * info)
{
	u32_t resp[4];
	u32_t arg;
	u32_t i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		arg = MMC_VDD_33_34;
		if(info->type == MMC_CARD_TYPE_SD20)
			arg |= 0x40000000;

		if(mmc_send_acmd(SD_APP_SEND_OP_COND, arg, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
			{
				if((resp[0] & 0x40000000) == 0x40000000)
					info->type = MMC_CARD_TYPE_SDHC;
				else if(info->type != MMC_CARD_TYPE_SD20)
					info->type = MMC_CARD_TYPE_SD;
				return TRUE;
			}
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

static bool_t mmc_send_op_cond(struct mmc_card_info * info)
{
	u32_t resp[4];
	u32_t i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		if(mmc_send_cmd(MMC_SEND_OP_COND, MMC_VDD_33_34 | 0x40000000, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
			{
				info->type = MMC_CARD_TYPE_MMC;
				return TRUE;
			}
		}
	}

	info->type = MMC_CARD_TYPE_NONE;
	return FALSE;
}

static void realview_mmc_init(void)
{
	/* power on and rod control */
	writel(REALVIEW_MCI_POWER, 0x83);

	writel(REALVIEW_MCI_MASK0, 0x0);
	writel(REALVIEW_MCI_MASK1, 0x0);

	writel(REALVIEW_MCI_COMMAND, 0x0);
	writel(REALVIEW_MCI_DATA_CTRL, 0x0);
}

static void realview_mmc_exit(void)
{
	writel(REALVIEW_MCI_MASK0, 0x0);
	writel(REALVIEW_MCI_MASK1, 0x0);

	writel(REALVIEW_MCI_COMMAND, 0x0);
	writel(REALVIEW_MCI_DATA_CTRL, 0x0);

	/* power off */
	writel(REALVIEW_MCI_POWER, 0x0);
}

static bool_t realview_mmc_probe(struct mmc_card_info * info)
{
	u32_t resp[4];
	bool_t ret;

	/*
	 * go idle mode
	 */
	if(!mmc_idle_cards())
		return FALSE;

	/*
	 * test for sd version 2
	 */
	mmc_send_if_cond(info);

	/* try to get the sd card's operating condition */
	if(!sd_send_op_cond(info))
	{
		if(!mmc_send_op_cond(info))
			return FALSE;
	}

	/*
	 * get the attached card's cid
	 */
	ret = mmc_send_cmd(MMC_ALL_SEND_CID, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->raw_cid[0] = resp[0];
	info->raw_cid[1] = resp[1];
	info->raw_cid[2] = resp[2];
	info->raw_cid[3] = resp[3];

	/*
	 * send relative card address
	 */
	switch(info->type)
	{
	case MMC_CARD_TYPE_MMC:
		info->rca = 0x0001;
		ret = mmc_send_cmd(MMC_SET_RELATIVE_ADDR, info->rca << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
		break;

	case MMC_CARD_TYPE_SD:
	case MMC_CARD_TYPE_SD20:
	case MMC_CARD_TYPE_SDHC:
		ret = mmc_send_cmd(SD_SEND_RELATIVE_ADDR, 0x0000 << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
		info->rca = (resp[0] >> 16) & 0xffff;
		break;

	default:
		info->rca = 0;
		return FALSE;
	}

	/*
	 * get the card specific data
	 */
	ret = mmc_send_cmd(MMC_SEND_CSD, (u32_t)(info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->raw_csd[0] = resp[0];
	info->raw_csd[1] = resp[1];
	info->raw_csd[2] = resp[2];
	info->raw_csd[3] = resp[3];

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

static bool_t realview_mmc_read_one_sector(struct mmc_card * card, u8_t * buf, u32_t sector)
{
	u32_t resp[4];
	u32_t blk_bits = card->info->csd.read_blkbits;
	u32_t blk_len = 1 << blk_bits;
	s32_t i, len, remain = blk_len;
	u8_t * p = buf;
	u32_t status;
	u32_t v;
	bool_t ret;

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(card->info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * always do full block reads from the card
	 */
	ret = mmc_send_cmd(MMC_SET_BLOCKLEN, blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	writel(REALVIEW_MCI_DATA_TIMER, 0xffff);
	writel(REALVIEW_MCI_DATA_LENGTH, blk_len);
	writel(REALVIEW_MCI_DATA_CTRL, (0x1<<0) | (0x1<<1) | (blk_bits<<4));

	if(card->info->type == MMC_CARD_TYPE_SDHC)
	{
		ret = mmc_send_cmd(MMC_READ_SINGLE_BLOCK, sector, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}
	else
	{
		ret = mmc_send_cmd(MMC_READ_SINGLE_BLOCK, sector * blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}

	do {
		len = remain - (readl(REALVIEW_MCI_FIFO_CNT) << 2);

		if(len <= 0)
			break;

		for(i = 0; i < (len >> 2); i++)
		{
			v = readl(REALVIEW_MCI_FIFO);
			*(p++) = (v >> 0) & 0xff;
			*(p++) = (v >> 8) & 0xff;
			*(p++) = (v >> 16) & 0xff;
			*(p++) = (v >> 24) & 0xff;
		}

		remain -= len;

		if(remain <= 0)
			break;

		status = readl(REALVIEW_MCI_STATUS);
		if((status & REALVIEW_MCI_STAT_DAT_TIME_OUT) || (status & REALVIEW_MCI_STAT_DAT_CRC_FAIL))
		{
			writel(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_DAT_TIMEOUT | REALVIEW_MCI_CLR_DAT_CRC_FAIL));

			mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
			return FALSE;
		}

	} while(1);

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

static bool_t realview_mmc_write_one_sector(struct mmc_card * card, const u8_t * buf, u32_t sector)
{
	u32_t resp[4];
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	s32_t i, remain = blk_len;
	u8_t * p = (u8_t *)buf;
	u32_t status;
	bool_t ret;

	/*
	 * select the card, and put it into transfer mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, (u32_t)(card->info->rca) << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	/*
	 * always do full block writes to the card
	 */
	ret = mmc_send_cmd(MMC_SET_BLOCKLEN, blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	writel(REALVIEW_MCI_DATA_TIMER, 0xffff);
	writel(REALVIEW_MCI_DATA_LENGTH, blk_len);
	writel(REALVIEW_MCI_DATA_CTRL, (0x1<<0) | (0x0<<1) | (blk_bits<<4));

	if(card->info->type == MMC_CARD_TYPE_SDHC)
	{
		ret = mmc_send_cmd(MMC_WRITE_SINGLE_BLOCK, sector, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}
	else
	{
		ret = mmc_send_cmd(MMC_WRITE_SINGLE_BLOCK, sector * blk_len, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}

	do {
		for(i=0; i<64; i++)
		{
			writeb(REALVIEW_MCI_FIFO, *(p++));
		}

		remain -= 64;

		if(remain <= 0)
			break;

		status = readl(REALVIEW_MCI_STATUS);
		if((status & REALVIEW_MCI_STAT_DAT_TIME_OUT) || (status & REALVIEW_MCI_STAT_DAT_CRC_FAIL))
		{
			writel(REALVIEW_MCI_CLEAR, (REALVIEW_MCI_CLR_DAT_TIMEOUT | REALVIEW_MCI_CLR_DAT_CRC_FAIL));

			mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
			return FALSE;
		}
	} while(1);

	/*
	 * deselect the card, and put it into standby mode
	 */
	ret = mmc_send_cmd(MMC_SELECT_CARD, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	return TRUE;
}

static bool_t realview_mmc_read_sectors(struct mmc_card * card, u8_t * buf, u32_t sector, u32_t count)
{
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	bool_t ret;

	while(count)
	{
		ret = realview_mmc_read_one_sector(card, buf, sector);
		if(ret != TRUE)
			return FALSE;

		count--;
		sector++;
		buf += blk_len;
	}

	return TRUE;
}

static bool_t realview_mmc_write_sectors(struct mmc_card * card, const u8_t * buf, u32_t sector, u32_t count)
{
	u32_t blk_bits = card->info->csd.write_blkbits;
	u32_t blk_len = 1 << blk_bits;
	bool_t ret;

	while(count)
	{
		ret = realview_mmc_write_one_sector(card, buf, sector);
		if(ret != TRUE)
			return FALSE;

		count--;
		sector++;
		buf += blk_len;
	}

	return TRUE;
}

static struct mmc_host realview_mmc_host_controller = {
	.name			= "realview-mmc",
	.init			= realview_mmc_init,
	.exit			= realview_mmc_exit,
	.probe			= realview_mmc_probe,
	.read_sectors	= realview_mmc_read_sectors,
	.write_sectors	= realview_mmc_write_sectors,
};

static __init void realview_mmc_host_controller_init(void)
{
	if(!register_mmc_host(&realview_mmc_host_controller))
		LOG_E("failed to register mmc host controller '%s'", realview_mmc_host_controller.name);
}

static __exit void realview_mmc_host_controller_exit(void)
{
	if(!unregister_mmc_host(&realview_mmc_host_controller))
		LOG_E("failed to unregister mmc host controller '%s'", realview_mmc_host_controller.name);
}

core_initcall(realview_mmc_host_controller_init);
core_exitcall(realview_mmc_host_controller_exit);
