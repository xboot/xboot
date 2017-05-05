/*
 * driver/sd/sdcard.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <sd/sdhci.h>
#include <sd/sdcard.h>

#define UNSTUFF_BITS(resp, start, size)								\
	({																\
		const int __size = size;									\
		const u32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);						\
		const int __shft = (start) & 31;							\
		u32_t __res;												\
																	\
		__res = resp[__off] >> __shft;								\
		if(__size + __shft > 32)									\
			__res |= resp[__off-1] << ((32 - __shft) % 32);			\
		__res & __mask;												\
	})

static const unsigned tran_speed_unit[] = {
	[0] = 10000,
	[1] = 100000,
	[2] = 1000000,
	[3] = 10000000,
};

static const unsigned char tran_speed_time[] = {
	0,
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

static bool_t mmc_go_idle(struct sdhci_t * sdhci)
{
	struct sdhci_cmd_t cmd;

	cmd.cmdidx = MMC_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_NONE;

	if(sdhci_transfer(sdhci, &cmd, NULL))
		return TRUE;
	udelay(2000);
	return sdhci_transfer(sdhci, &cmd, NULL);
}

static bool_t sd_send_if_cond(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	cmd.cmdarg = ((sdhci->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resptype = MMC_RSP_R7;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;

 	if((cmd.response[0] & 0xff) == 0xaa)
 	{
 	 	sdcard->version = SD_VERSION_2;
 	 	return TRUE;
 	}
 	return FALSE;
}

static bool_t sd_send_op_cond(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;
	int timeout = 100;

	do {
		cmd.cmdidx = MMC_APP_CMD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
	 	if(!sdhci_transfer(sdhci, &cmd, NULL))
	 		return FALSE;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.cmdarg = sdhci->voltages & 0xff8000;
		if(sdcard->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;
		cmd.resptype = MMC_RSP_R3;
	 	if(!sdhci_transfer(sdhci, &cmd, NULL))
	 		return FALSE;

	 	udelay(1000);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if(timeout <= 0)
		return FALSE;

	if(sdcard->version != SD_VERSION_2)
		sdcard->version = SD_VERSION_1_0;
	sdcard->ocr = cmd.response[0];
	sdcard->high_capacity = ((sdcard->ocr & OCR_HCS) == OCR_HCS);
	sdcard->rca = 0;

	return TRUE;
}

static bool_t mmc_send_op_cond(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;
	int timeout = 100;

	if(!mmc_go_idle(sdhci))
		return FALSE;

	cmd.cmdidx = MMC_SEND_OP_COND;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_R3;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;

	do {
		cmd.cmdidx = MMC_SEND_OP_COND;
		cmd.cmdarg = OCR_HCS | (sdhci->voltages & (sdcard->ocr & OCR_VOLTAGE_MASK)) | (sdcard->ocr & OCR_ACCESS_MODE);
		cmd.resptype = MMC_RSP_R3;
	 	if(!sdhci_transfer(sdhci, &cmd, NULL))
	 		return FALSE;
	 	udelay(1000);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if(timeout <= 0)
		return FALSE;

	sdcard->version = MMC_VERSION_UNKNOWN;
	sdcard->ocr = cmd.response[0];
	sdcard->high_capacity = ((sdcard->ocr & OCR_HCS) == OCR_HCS);
	sdcard->rca = 0;
	return TRUE;
}

static bool_t sdcard_detect(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;
	struct sdhci_data_t dat;
	u64_t csize, cmult;
	u32_t unit, time;
	bool_t ret;

	if(!sdhci_detect(sdhci))
		return FALSE;

	sdhci_reset(sdhci);
	sdhci_set_width(sdhci, MMC_BUS_WIDTH_1);
	sdhci_set_clock(sdhci, 400000);

	if(!mmc_go_idle(sdhci))
		return FALSE;

	ret = sd_send_if_cond(sdhci, sdcard);
	ret = sd_send_op_cond(sdhci, sdcard);
	if(!ret)
		ret = mmc_send_op_cond(sdhci, sdcard);
	if(!ret)
		return FALSE;

	cmd.cmdidx = MMC_ALL_SEND_CID;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_R2;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;
 	sdcard->cid[0] = cmd.response[0];
 	sdcard->cid[1] = cmd.response[1];
 	sdcard->cid[2] = cmd.response[2];
 	sdcard->cid[3] = cmd.response[3];

	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = sdcard->rca << 16;
	cmd.resptype = MMC_RSP_R6;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;
	if(IS_SD(sdcard))
		sdcard->rca = (cmd.response[0] >> 16) & 0xffff;

	cmd.cmdidx = MMC_SEND_CSD;
	cmd.cmdarg = sdcard->rca << 16;
	cmd.resptype = MMC_RSP_R2;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;
 	sdcard->csd[0] = cmd.response[0];
 	sdcard->csd[1] = cmd.response[1];
 	sdcard->csd[2] = cmd.response[2];
 	sdcard->csd[3] = cmd.response[3];

	if(sdcard->version == MMC_VERSION_UNKNOWN)
	{
		switch((sdcard->csd[0] >> 26) & 0xf)
		{
		case 0:
			sdcard->version = MMC_VERSION_1_2;
			break;
		case 1:
			sdcard->version = MMC_VERSION_1_4;
			break;
		case 2:
			sdcard->version = MMC_VERSION_2_2;
			break;
		case 3:
			sdcard->version = MMC_VERSION_3;
			break;
		case 4:
			sdcard->version = MMC_VERSION_4;
			break;
		default:
			sdcard->version = MMC_VERSION_1_2;
			break;
		};
	}

	unit = tran_speed_unit[(sdcard->csd[0] & 0x7)];
	time = tran_speed_time[((sdcard->csd[0] >> 3) & 0xf)];
	sdcard->tran_speed = time * unit;
	sdcard->dsr_imp = UNSTUFF_BITS(sdcard->csd, 76, 1);

	sdcard->read_bl_len = 1 << UNSTUFF_BITS(sdcard->csd, 80, 4);
	if(IS_SD(sdcard))
		sdcard->write_bl_len = sdcard->read_bl_len;
	else
		sdcard->write_bl_len = 1 << ((sdcard->csd[3] >> 22) & 0xf);
	if(sdcard->read_bl_len > 512)
		sdcard->read_bl_len = 512;
	if(sdcard->write_bl_len > 512)
		sdcard->write_bl_len = 512;

	cmd.cmdidx = MMC_SELECT_CARD;
	cmd.cmdarg = sdcard->rca << 16;
	cmd.resptype = MMC_RSP_R1;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;

	if(!IS_SD(sdcard) && (sdcard->version >= MMC_VERSION_4))
	{
		cmd.cmdidx = MMC_SEND_EXT_CSD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;

		dat.buf = sdcard->extcsd;
		dat.flag = MMC_DATA_READ;
		dat.blksz = 512;
		dat.blkcnt = 1;

	 	if(!sdhci_transfer(sdhci, &cmd, &dat))
	 		return FALSE;

		switch(sdcard->extcsd[192])
		{
		case 1:
			sdcard->version = MMC_VERSION_4_1;
			break;
		case 2:
			sdcard->version = MMC_VERSION_4_2;
			break;
		case 3:
			sdcard->version = MMC_VERSION_4_3;
			break;
		case 5:
			sdcard->version = MMC_VERSION_4_41;
			break;
		case 6:
			sdcard->version = MMC_VERSION_4_5;
			break;
		case 7:
			sdcard->version = MMC_VERSION_5_0;
			break;
		case 8:
			sdcard->version = MMC_VERSION_5_1;
			break;
		default:
			break;
		}
	}

	if(sdcard->high_capacity)
	{
		if(IS_SD(sdcard))
		{
			csize = UNSTUFF_BITS(sdcard->csd, 48, 22);
			sdcard->capacity = (1 + csize) << 10;
		}
		else
		{
			sdcard->capacity = sdcard->extcsd[212] << 0 |
				sdcard->extcsd[212 + 1] << 8 |
				sdcard->extcsd[212 + 2] << 16 |
				sdcard->extcsd[212 + 3] << 24;
		}
	}
	else
	{
		cmult = UNSTUFF_BITS(sdcard->csd, 47, 3);
		csize = UNSTUFF_BITS(sdcard->csd, 62, 12);
		sdcard->capacity = (csize + 1) << (cmult + 2);
	}
	sdcard->capacity *= 1 << UNSTUFF_BITS(sdcard->csd, 80, 4);

/*	if(IS_SD(sdcard))
		ret = sd_change_freq(sdcard);
	else
		ret = mmc_change_freq(sdcard);
	if(!ret)
		return FALSE;

	if(IS_SD(sdcard))
		ret = mci_startup_sd(sdcard);
	else
		ret = mci_startup_mmc(sdcard);
	if(!ret)
		return FALSE;
*/

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = sdcard->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sdhci_transfer(sdhci, &cmd, NULL))
		return FALSE;

	return TRUE;
}

struct sdcard_pdata_t
{
	struct sdcard_t sdcard;
	struct timer_t timer;
};

static u64_t disk_sdcard_read(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	return 0;
}

static u64_t disk_sdcard_write(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	return 0;
}

static void disk_sdcard_sync(struct disk_t * disk)
{
}

void sdcard_auto_probe(struct sdhci_t * sdhci)
{
	struct sdcard_pdata_t * pdat;
	struct disk_t * disk;

	pdat = malloc(sizeof(struct sdcard_pdata_t));
	if(!pdat)
		return;

	if(sdcard_detect(sdhci, &pdat->sdcard))
	{
		disk = malloc(sizeof(struct disk_t));
		if(disk)
		{
			disk->name = alloc_device_name(sdhci->name, -1);
			disk->size = pdat->sdcard.read_bl_len;
			disk->count = pdat->sdcard.capacity / pdat->sdcard.read_bl_len;
			disk->read = disk_sdcard_read;
			disk->write = disk_sdcard_write;
			disk->sync = disk_sdcard_sync;
			disk->priv = pdat;

			if(!register_disk(&dev, disk))
			{
				free_device_name(disk->name);
				free(blk->priv);
				free(disk);
			}

		}
	}

	if(sdhci->removeable)
	{

	}

//	memset(&sdcard, 0, sizeof(struct sdcard_t));

}

void sdcard_test(void)
{
	struct sdhci_t * sdhci = search_sdhci("sdhci-pl180.0");
	struct sdcard_t sdcard;

	memset(&sdcard, 0, sizeof(struct sdcard_t));
	printf("r = %d\r\n", sdcard_detect(sdhci, &sdcard));
}

#if 0
static struct timer_t sdcard_timer;

static int sdcard_detect_timer_function(struct timer_t * timer, void * data)
{
	timer_forward_now(timer, ms_to_ktime(2000));
	return 1;
}

static __init void sdcard_server_init(void)
{
	timer_init(&sdcard_timer, sdcard_detect_timer_function, NULL);
	timer_start_now(&sdcard_timer, ms_to_ktime(2000));
}

static __exit void sdcard_server_exit(void)
{
	timer_cancel(&sdcard_timer);
}

server_initcall(sdcard_server_init);
server_exitcall(sdcard_server_exit);
#endif
