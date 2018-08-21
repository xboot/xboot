/*
 * driver/sd/sdcard.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <block/disk.h>
#include <sd/sdhci.h>
#include <sd/sdcard.h>

struct sdcard_t
{
	u32_t version;
	u32_t ocr;
	u32_t rca;
	u32_t cid[4];
	u32_t csd[4];
	u8_t extcsd[512];

	u32_t high_capacity;
	u32_t tran_speed;
	u32_t dsr_imp;
	u32_t read_bl_len;
	u32_t write_bl_len;
	u64_t capacity;
};

struct sdcard_pdata_t
{
	struct disk_t disk;
	struct sdcard_t sdcard;
	struct timer_t timer;
	struct sdhci_t * sdhci;
	bool_t online;
};

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

static u64_t mmc_read_blocks(struct sdhci_t * sdhci, struct sdcard_t * sdcard, u8_t * buf, u64_t start, u64_t blkcnt)
{
	struct sdhci_cmd_t cmd;
	struct sdhci_data_t dat;

	if(blkcnt > 1)
		cmd.cmdidx = MMC_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_READ_SINGLE_BLOCK;
	if(sdcard->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * sdcard->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	dat.buf = buf;
	dat.flag = MMC_DATA_READ;
	dat.blksz = sdcard->read_bl_len;
	dat.blkcnt = blkcnt;
	if(!sdhci_transfer(sdhci, &cmd, &dat))
		return 0;

	if(blkcnt > 1)
	{
		cmd.cmdidx = MMC_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1B;
		if(!sdhci_transfer(sdhci, &cmd, NULL))
			return 0;
	}
	return blkcnt;
}

static u64_t mmc_write_blocks(struct sdhci_t * sdhci, struct sdcard_t * sdcard, u8_t * buf, u64_t start, u64_t blkcnt)
{
	struct sdhci_cmd_t cmd;
	struct sdhci_data_t dat;

	if(blkcnt == 0)
		return 0;
	else if(blkcnt == 1)
		cmd.cmdidx = MMC_WRITE_SINGLE_BLOCK;
	else
		cmd.cmdidx = MMC_WRITE_MULTIPLE_BLOCK;
	if(sdcard->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * sdcard->write_bl_len;
	cmd.resptype = MMC_RSP_R1;
	dat.buf = buf;
	dat.flag = MMC_DATA_WRITE;
	dat.blksz = sdcard->write_bl_len;
	dat.blkcnt = blkcnt;
	if(!sdhci_transfer(sdhci, &cmd, &dat))
		return 0;

	if(blkcnt > 1)
	{
		cmd.cmdidx = MMC_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1B;
		if(!sdhci_transfer(sdhci, &cmd, NULL))
			return 0;
	}
	return blkcnt;
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
		return FALSE; */

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = sdcard->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sdhci_transfer(sdhci, &cmd, NULL))
		return FALSE;

	return TRUE;
}

static u64_t sdcard_disk_read(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	struct sdcard_pdata_t * pdat = (struct sdcard_pdata_t *)(disk->priv);
	struct sdhci_t * sdhci = pdat->sdhci;
	struct sdcard_t * sdcard = &pdat->sdcard;
	struct sdhci_cmd_t cmd;
	u64_t cnt, blks = count;

	if(count == 0)
		return 0;

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = sdcard->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sdhci_transfer(sdhci, &cmd, NULL))
		return 0;

	do {
		cnt = (blks > 65535) ?  65535 : blks;
		if(mmc_read_blocks(sdhci, sdcard, buf, sector, cnt) != cnt)
			return 0;
		blks -= cnt;
		sector += cnt;
		buf += cnt * sdcard->read_bl_len;
	} while(blks > 0);

	return count;
}

static u64_t sdcard_disk_write(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	struct sdcard_pdata_t * pdat = (struct sdcard_pdata_t *)(disk->priv);
	struct sdhci_t * sdhci = pdat->sdhci;
	struct sdcard_t * sdcard = &pdat->sdcard;
	struct sdhci_cmd_t cmd;
	u64_t cnt, blks = count;

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = sdcard->write_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sdhci_transfer(sdhci, &cmd, NULL))
		return 0;

	do {
		cnt = (blks > 65535) ?  65535 : blks;
		if(mmc_write_blocks(sdhci, sdcard, buf, sector, cnt) != cnt)
			return 0;
		blks -= cnt;
		sector += cnt;
		buf += cnt * sdcard->write_bl_len;
	} while (blks > 0);

	return count;
}

static void sdcard_disk_sync(struct disk_t * disk)
{
}

static int sdcard_disk_timer_function(struct timer_t * timer, void * data)
{
	struct sdcard_pdata_t * pdat = (struct sdcard_pdata_t *)(data);
	char buf[64];
	bool_t plugin;

	if(!pdat->sdhci->removeable)
		return 0;

	plugin = sdhci_detect(pdat->sdhci);
	if(pdat->online && !plugin)
	{
		if(unregister_disk(&pdat->disk))
		{
			free_device_name(pdat->disk.name);
			pdat->online = FALSE;
		}
	}
	else if(!pdat->online && plugin)
	{
		if(sdcard_detect(pdat->sdhci, &pdat->sdcard))
		{
			snprintf(buf, sizeof(buf), "sdcard.%s", pdat->sdhci->name);
			pdat->disk.name = strdup(buf);
			pdat->disk.size = pdat->sdcard.read_bl_len;
			pdat->disk.count = pdat->sdcard.capacity / pdat->sdcard.read_bl_len;
			pdat->disk.read = sdcard_disk_read;
			pdat->disk.write = sdcard_disk_write;
			pdat->disk.sync = sdcard_disk_sync;
			pdat->disk.priv = pdat;
			if(!register_disk(NULL, &pdat->disk))
				free_device_name(pdat->disk.name);
			else
				pdat->online = TRUE;
		}
	}
	timer_forward_now(timer, ms_to_ktime(2000));
	return 1;
}

void * sdcard_probe(struct sdhci_t * sdhci)
{
	struct sdcard_pdata_t * pdat;

	pdat = malloc(sizeof(struct sdcard_pdata_t));
	if(!pdat)
		return NULL;
	memset(pdat, 0, sizeof(struct sdcard_pdata_t));

	pdat->sdhci = sdhci;
	pdat->online = FALSE;
	timer_init(&pdat->timer, sdcard_disk_timer_function, pdat);
	timer_start_now(&pdat->timer, ms_to_ktime(100));

	return pdat;
}

void sdcard_remove(void * sdcard)
{
	struct sdcard_pdata_t * pdat = (struct sdcard_pdata_t *)sdcard;

	if(pdat)
	{
		timer_cancel(&pdat->timer);
		if(pdat->online && unregister_disk(&pdat->disk))
			free_device_name(pdat->disk.name);
		free(pdat);
	}
}
