/*
 * arch/arm/mach-realview/realview-mmc.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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


#include <configs.h>
#include <default.h>
#include <macros.h>
#include <types.h>
#include <div64.h>
#include <xboot/log.h>
#include <xboot/io.h>
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

/*
 * card type
 */
enum realview_mci_card_type {
	REALVIEW_MCI_SD_CARD,
	REALVIEW_MCI_MMC_CARD,
};

static x_bool mmc_send_cmd(x_u32 cmd, x_u32 arg, x_u32 * resp, x_u32 flags)
{
	x_u32 status;
	x_bool ret = TRUE;

	if(readl(REALVIEW_MCI_COMMAND) & REALVIEW_MCI_CMD_ENABLE)
	{
		writel(REALVIEW_MCI_COMMAND, 0x0);
		udelay(1);
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

static x_bool mmc_send_acmd(x_u32 cmd, x_u32 arg, x_u32 * resp, x_u32 flags)
{
	x_u32 aresp;
	x_bool ret = TRUE;

	ret = mmc_send_cmd(MMC_APP_CMD, 0, &aresp, REALVIEW_MCI_RSP_PRESENT);
	if(!ret)
		return ret;

	if((aresp & ((1 << 22) | (1 << 5))) != (1 << 5))
		return FALSE;

	return mmc_send_cmd(cmd, arg, resp, flags);
}

static x_bool mmc_idle_cards(void)
{
	x_bool ret = TRUE;

	/*
	 * reset all cards
	 */
	ret = mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, REALVIEW_MCI_RSP_NONE);
	if(ret)
		return ret;

	/*
	 * wait a moment
	 */
	udelay(2000);

	return mmc_send_cmd(MMC_GO_IDLE_STATE, 0, NULL, 0);
}

static x_bool sd_set_ocr(void)
{
	x_u32 resp[4];
	x_u32 i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		if(mmc_send_acmd(SD_APP_SEND_OP_COND, MMC_VDD_33_34, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
				return TRUE;
		}
	}

	return FALSE;
}

static x_bool mmc_set_ocr(void)
{
	x_u32 resp[4];
	x_u32 i;

	if(!mmc_idle_cards())
		return FALSE;

	for(i=0; i<100; i++)
	{
		if(mmc_send_cmd(MMC_SEND_OP_COND, MMC_VDD_33_34, &resp[0], REALVIEW_MCI_RSP_PRESENT))
		{
			if(resp[0] & 0x80000000)
				return TRUE;
		}
	}

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

x_bool realview_mmc_probe(struct mmc_card_info * info)
{
	x_u32 resp[4];
	enum realview_mci_card_type type;
	x_u32 rca;
	x_bool ret;

	/*
	 * enter idle mode
	 */
	if(!mmc_idle_cards())
		return FALSE;

	if(sd_set_ocr())
		type = REALVIEW_MCI_SD_CARD;
	else if(mmc_set_ocr())
		type = REALVIEW_MCI_MMC_CARD;
	else
		return FALSE;

	/*
	 * get the attached card's cid
	 */
	ret = mmc_send_cmd(MMC_ALL_SEND_CID, 0, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->cid.mid = (resp[0] >> 24) & 0xff;

	info->cid.oid[0] = (resp[0] >> 16) & 0xff;
	info->cid.oid[1] = (resp[0] >> 8) & 0xff;

	info->cid.pnm[0] = (resp[0] >> 0) & 0xff;
	info->cid.pnm[1] = (resp[1] >> 24) & 0xff;
	info->cid.pnm[2] = (resp[1] >> 16) & 0xff;
	info->cid.pnm[3] = (resp[1] >> 8) & 0xff;
	info->cid.pnm[4] = (resp[1] >> 0) & 0xff;

	info->cid.prev = (resp[2] >> 24) & 0xff;

	info->cid.psn[0] = (resp[2] >> 16) & 0xff;
	info->cid.psn[1] = (resp[2] >> 8) & 0xff;
	info->cid.psn[2] = (resp[2] >> 0) & 0xff;
	info->cid.psn[3] = (resp[3] >> 24) & 0xff;

	info->cid.year = (((resp[3] >> 16) & 0xff) * 10) + 2000 + ((((resp[3] >> 8) & 0xff) >> 4) & 0x0f);
	info->cid.month = ((resp[3] >> 8) & 0xff) & 0x0f;

	/*
	 * send relative card address
	 */
	if(type == REALVIEW_MCI_SD_CARD)
	{
		ret = mmc_send_cmd(MMC_SET_RELATIVE_ADDR, 0x0000 << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
		rca = (resp[0] >> 16) & 0xffff;
	}
	else if(type == REALVIEW_MCI_MMC_CARD)
	{
		rca = 0x0001;
		ret = mmc_send_cmd(MMC_SET_RELATIVE_ADDR, rca << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_CRC);
		if(!ret)
			return FALSE;
	}
	else
	{
		return FALSE;
	}

	/*
	 * get the card specific data
	 */
	ret = mmc_send_cmd(MMC_SEND_CSD, rca << 16, resp, REALVIEW_MCI_RSP_PRESENT | REALVIEW_MCI_RSP_136BIT | REALVIEW_MCI_RSP_CRC);
	if(!ret)
		return FALSE;

	info->csd.sector_size = 1 << ((resp[1] >> 16) & 0xf);
	info->csd.sector_count = 100;
	info->csd.capacity = info->csd.sector_size * info->csd.sector_count;


	int n = ((resp[1] >> 16) & 0xf) + ((resp[2] >> 8) & 0x7f) + (((resp[2] >> 16) & 0x3) << 1) + 2;
	int csize = ((resp[2] >> 24) >> 6) + ((resp[2] & 0xff) << 2) + (((resp[2] >> 8) & 0x3) << 10) + 1;
	int t = (unsigned long)csize << (n - 9);

	LOG_I("n=%ld", n);
	LOG_I("csize=%ld", csize);
	LOG_I("t=%ld", t);

	LOG_I("sector_size=%ld", info->csd.sector_size);

	LOG_I("mid=0x%lx", info->cid.mid);
	LOG_I("oid=%c,%c", info->cid.oid[0],info->cid.oid[1]);

	LOG_I("pnm=%c,%c,%c,%c,%c", info->cid.pnm[0],info->cid.pnm[1],info->cid.pnm[2],info->cid.pnm[3],info->cid.pnm[4]);
	LOG_I("prev=0x%lx", info->cid.prev);

	LOG_I("psn=%x,%x,%x,%x", info->cid.psn[0],info->cid.psn[1],info->cid.psn[2],info->cid.psn[3]);
	LOG_I("y,m=0x%ld, 0x%ld", info->cid.year, info->cid.month);

	LOG_I("0x%lx,0x%lx,0x%lx,0x%lx", resp[0], resp[1], resp[2], resp[3]);

	LOG_I("rca=0x%lx", rca);

	return TRUE;
}

x_bool realview_mmc_read_sector(struct mmc_card * card, x_u32 sector, x_u8 * data)
{
	return FALSE;
}

x_bool realview_mmc_write_sector(struct mmc_card * nand, x_u32 sector, x_u8 * data)
{
	return FALSE;
}

static struct mmc_host realview_mmc_host_controller = {
	.name			= "realview-mmc",
	.init			= realview_mmc_init,
	.exit			= realview_mmc_exit,
	.probe			= realview_mmc_probe,
	.read_sector	= realview_mmc_read_sector,
	.write_sector	= realview_mmc_write_sector,
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

module_init(realview_mmc_host_controller_init, LEVEL_MACH_RES);
module_exit(realview_mmc_host_controller_exit, LEVEL_MACH_RES);
