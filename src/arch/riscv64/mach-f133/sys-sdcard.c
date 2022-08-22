/*
 * sys-sdcard.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <sd/sdmmc.h>

struct sdcard_info_t {
	uint32_t version;
	uint32_t ocr;
	uint32_t rca;
	uint32_t cid[4];
	uint32_t csd[4];
	uint8_t extcsd[512];

	uint32_t high_capacity;
	uint32_t tran_speed;
	uint32_t dsr_imp;
	uint32_t read_bl_len;
	uint32_t write_bl_len;
	uint64_t capacity;
};

struct sdhci_cmd_t {
	u32_t cmdidx;
	u32_t cmdarg;
	u32_t resptype;
	u32_t response[4];
};

struct sdhci_data_t {
	u8_t * buf;
	u32_t flag;
	u32_t blksz;
	u32_t blkcnt;
};

extern void sys_sdhci_reset(void);
extern void sys_sdhci_setclock(uint32_t clock);
extern void sys_sdhci_setwidth(uint32_t width);
extern int sys_sdhci_transfer_command(struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);
extern int sys_sdhci_transfer_data(struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat);

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define UNSTUFF_BITS(resp, start, size)									\
	({																	\
		const int __size = size;										\
		const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);							\
		const int __shft = (start) & 31;								\
		uint32_t __res;													\
																		\
		__res = resp[__off] >> __shft;									\
		if(__size + __shft > 32)										\
			__res |= resp[__off-1] << ((32 - __shft) % 32);				\
		__res & __mask;													\
	})

static const unsigned tran_speed_unit[] = {
	[0] = 10000,
	[1] = 100000,
	[2] = 1000000,
	[3] = 10000000,
};

static const unsigned char tran_speed_time[] = {
	 0, 10, 12, 13, 15, 20, 25, 30,
	35, 40, 45, 50, 55, 60, 70, 80,
};

static int go_idle_state(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };

	cmd.cmdidx = MMC_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_NONE;
	if(sys_sdhci_transfer_command(&cmd, NULL))
		return 1;
	return sys_sdhci_transfer_command(&cmd, NULL);
}

static int sd_send_if_cond(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	cmd.cmdarg = (0x1 << 8);
	cmd.cmdarg |= 0xaa;
	cmd.resptype = MMC_RSP_R7;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	if((cmd.response[0] & 0xff) != 0xaa)
		return 0;
	info->version = SD_VERSION_2;
	return 1;
}

static int sd_send_op_cond(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };
	int retries = 100;

	do {
		cmd.cmdidx = MMC_APP_CMD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			continue;
		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.cmdarg = 0x00ff8000;
		if(info->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;
		cmd.resptype = MMC_RSP_R3;
		if(!sys_sdhci_transfer_command(&cmd, NULL) || (cmd.response[0] & OCR_BUSY))
			break;
	} while(retries--);
	if(retries <= 0)
		return 0;
	if(info->version != SD_VERSION_2)
		info->version = SD_VERSION_1_0;
	info->ocr = cmd.response[0];
	info->high_capacity = ((info->ocr & OCR_HCS) == OCR_HCS);
	info->rca = 0;
	return 1;
}

static int mmc_send_op_cond(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };
	int retries = 100;

	if(!go_idle_state(info))
		return 0;
	cmd.cmdidx = MMC_SEND_OP_COND;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_R3;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	do {
		cmd.cmdidx = MMC_SEND_OP_COND;
		cmd.cmdarg = (info->ocr & OCR_VOLTAGE_MASK) | (info->ocr & OCR_ACCESS_MODE);
		cmd.cmdarg |= OCR_HCS;
		cmd.resptype = MMC_RSP_R3;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
	} while (!(cmd.response[0] & OCR_BUSY) && retries--);
	if(retries <= 0)
		return 0;
	info->version = MMC_VERSION_UNKNOWN;
	info->ocr = cmd.response[0];
	info->high_capacity = ((info->ocr & OCR_HCS) == OCR_HCS);
	info->rca = 0;
	return 1;
}

static int mmc_status(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };
	int retries = 100;

	cmd.cmdidx = MMC_SEND_STATUS;
	cmd.resptype = MMC_RSP_R1;
	cmd.cmdarg = info->rca << 16;
	do
	{
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			continue;
		if(cmd.response[0] & (1 << 8))
			break;
		usleep(1);
	} while(retries-- > 0);
	if(retries > 0)
		return ((cmd.response[0] >> 9) & 0xf);
	return -1;
}

static uint64_t mmc_read_blocks(struct sdcard_info_t * info, uint8_t * buf, uint64_t start, uint64_t blkcnt)
{
	struct sdhci_cmd_t cmd = { 0 };
	struct sdhci_data_t dat = { 0 };
	int status;

	if(blkcnt > 1)
		cmd.cmdidx = MMC_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_READ_SINGLE_BLOCK;
	if(info->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * info->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	dat.buf = buf;
	dat.flag = MMC_DATA_READ;
	dat.blksz = info->read_bl_len;
	dat.blkcnt = blkcnt;
	if(!sys_sdhci_transfer_data(&cmd, &dat))
		return 0;
	do {
		status = mmc_status(info);
		if(status < 0)
			return 0;
	} while((status != MMC_STATUS_TRAN) && (status != MMC_STATUS_DATA));
	if(blkcnt > 1)
	{
		cmd.cmdidx = MMC_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1B;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
	}
	return blkcnt;
}

static uint64_t sdcard_blk_read(struct sdcard_info_t * info, uint8_t * buf, uint64_t blkno, uint64_t blkcnt)
{
	uint64_t cnt, blks = blkcnt;

	while(blks > 0)
	{
		cnt = (blks > 127) ? 127 : blks;
		if(mmc_read_blocks(info, buf, blkno, cnt) != cnt)
			return 0;
		blks -= cnt;
		blkno += cnt;
		buf += cnt * info->read_bl_len;
	}
	return blkcnt;
}

static uint64_t sdcard_helper_read(struct sdcard_info_t * info, uint64_t offset, uint8_t * buf, uint64_t count)
{
	uint64_t blksz = info->read_bl_len;
	uint64_t blkno, len, tmp;
	uint64_t ret = 0;

	blkno = offset / blksz;
	tmp = offset % blksz;
	if(tmp > 0)
	{
		len = blksz - tmp;
		if(count < len)
			len = count;
		if(sdcard_blk_read(info, &buf[0], blkno, 1) != 1)
			return ret;
		memcpy((void *)buf, (const void *)(&buf[tmp]), len);
		buf += len;
		count -= len;
		ret += len;
		blkno += 1;
	}
	tmp = count / blksz;
	if(tmp > 0)
	{
		len = tmp * blksz;
		if(sdcard_blk_read(info, buf, blkno, tmp) != tmp)
			return ret;
		buf += len;
		count -= len;
		ret += len;
		blkno += tmp;
	}
	if(count > 0)
	{
		len = count;
		if(sdcard_blk_read(info, &buf[0], blkno, 1) != 1)
			return ret;
		ret += len;
	}
	return ret;
}

static inline int sdcard_info(struct sdcard_info_t * info)
{
	struct sdhci_cmd_t cmd = { 0 };
	struct sdhci_data_t dat = { 0 };
	uint64_t csize, cmult;
	uint32_t unit, time;
	int status;

	sys_sdhci_reset();
	sys_sdhci_setclock(400 * 1000);
	sys_sdhci_setwidth(MMC_BUS_WIDTH_1);

	if(!go_idle_state(info))
		return 0;
	sd_send_if_cond(info);
	if(!sd_send_op_cond(info))
	{
		if(!mmc_send_op_cond(info))
			return 0;
	}

	cmd.cmdidx = MMC_ALL_SEND_CID;
	cmd.cmdarg = 0;
	cmd.resptype = MMC_RSP_R2;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;

	info->cid[0] = cmd.response[0];
	info->cid[1] = cmd.response[1];
	info->cid[2] = cmd.response[2];
	info->cid[3] = cmd.response[3];

	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = info->rca << 16;
	cmd.resptype = MMC_RSP_R6;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	if(info->version & SD_VERSION_SD)
		info->rca = (cmd.response[0] >> 16) & 0xffff;

	cmd.cmdidx = MMC_SEND_CSD;
	cmd.cmdarg = info->rca << 16;
	cmd.resptype = MMC_RSP_R2;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	info->csd[0] = cmd.response[0];
	info->csd[1] = cmd.response[1];
	info->csd[2] = cmd.response[2];
	info->csd[3] = cmd.response[3];

	cmd.cmdidx = MMC_SELECT_CARD;
	cmd.cmdarg = info->rca << 16;
	cmd.resptype = MMC_RSP_R1;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	do {
		status = mmc_status(info);
		if(status < 0)
			return 0;
	} while(status != MMC_STATUS_TRAN);

	if(info->version == MMC_VERSION_UNKNOWN)
	{
		switch((info->csd[0] >> 26) & 0xf)
		{
		case 0:
			info->version = MMC_VERSION_1_2;
			break;
		case 1:
			info->version = MMC_VERSION_1_4;
			break;
		case 2:
			info->version = MMC_VERSION_2_2;
			break;
		case 3:
			info->version = MMC_VERSION_3;
			break;
		case 4:
			info->version = MMC_VERSION_4;
			break;
		default:
			info->version = MMC_VERSION_1_2;
			break;
		};
	}

	unit = tran_speed_unit[(info->csd[0] & 0x7)];
	time = tran_speed_time[((info->csd[0] >> 3) & 0xf)];
	info->tran_speed = time * unit;
	info->dsr_imp = UNSTUFF_BITS(info->csd, 76, 1);

	info->read_bl_len = 1 << UNSTUFF_BITS(info->csd, 80, 4);
	if(info->version & SD_VERSION_SD)
		info->write_bl_len = info->read_bl_len;
	else
		info->write_bl_len = 1 << ((info->csd[3] >> 22) & 0xf);
	if(info->read_bl_len > 512)
		info->read_bl_len = 512;
	if(info->write_bl_len > 512)
		info->write_bl_len = 512;

	if((info->version & MMC_VERSION_MMC) && (info->version >= MMC_VERSION_4))
	{
		cmd.cmdidx = MMC_SEND_EXT_CSD;
		cmd.cmdarg = 0;
		cmd.resptype = MMC_RSP_R1;
		dat.buf = info->extcsd;
		dat.flag = MMC_DATA_READ;
		dat.blksz = 512;
		dat.blkcnt = 1;
		if(!sys_sdhci_transfer_data(&cmd, &dat))
			return 0;
		do {
			status = mmc_status(info);
			if(status < 0)
				return 0;
		} while(status != MMC_STATUS_TRAN);
		switch(info->extcsd[192])
		{
		case 1:
			info->version = MMC_VERSION_4_1;
			break;
		case 2:
			info->version = MMC_VERSION_4_2;
			break;
		case 3:
			info->version = MMC_VERSION_4_3;
			break;
		case 5:
			info->version = MMC_VERSION_4_41;
			break;
		case 6:
			info->version = MMC_VERSION_4_5;
			break;
		case 7:
			info->version = MMC_VERSION_5_0;
			break;
		case 8:
			info->version = MMC_VERSION_5_1;
			break;
		default:
			break;
		}
	}

	if(info->high_capacity)
	{
		if(info->version & SD_VERSION_SD)
		{
			csize = UNSTUFF_BITS(info->csd, 48, 22);
			info->capacity = (1 + csize) << 10;
		}
		else
		{
			info->capacity = info->extcsd[212] << 0 | info->extcsd[212 + 1] << 8 | info->extcsd[212 + 2] << 16 | info->extcsd[212 + 3] << 24;
		}
	}
	else
	{
		cmult = UNSTUFF_BITS(info->csd, 47, 3);
		csize = UNSTUFF_BITS(info->csd, 62, 12);
		info->capacity = (csize + 1) << (cmult + 2);
	}
	info->capacity *= 1 << UNSTUFF_BITS(info->csd, 80, 4);

	if(info->version & SD_VERSION_SD)
	{
		cmd.cmdidx = MMC_APP_CMD;
		cmd.cmdarg = info->rca << 16;
		cmd.resptype = MMC_RSP_R5;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
		cmd.cmdidx = SD_CMD_SWITCH_FUNC;
		cmd.cmdarg = 2;
		cmd.resptype = MMC_RSP_R1;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
		sys_sdhci_setclock(MIN(info->tran_speed, (uint32_t)(25 * 1000 * 1000)));
		sys_sdhci_setwidth(MMC_BUS_WIDTH_4);
	}
	else if(info->version & MMC_VERSION_MMC)
	{
		cmd.cmdidx = MMC_APP_CMD;
		cmd.cmdarg = info->rca << 16;
		cmd.resptype = MMC_RSP_R5;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
		cmd.cmdidx = SD_CMD_SWITCH_FUNC;
		cmd.cmdarg = 2;
		cmd.resptype = MMC_RSP_R1;
		if(!sys_sdhci_transfer_command(&cmd, NULL))
			return 0;
		sys_sdhci_setclock(MIN(info->tran_speed, (uint32_t)(50 * 1000 * 1000)));
		sys_sdhci_setwidth(MMC_BUS_WIDTH_8);
	}

	cmd.cmdidx = MMC_SET_BLOCKLEN;
	cmd.cmdarg = info->read_bl_len;
	cmd.resptype = MMC_RSP_R1;
	if(!sys_sdhci_transfer_command(&cmd, NULL))
		return 0;
	return 1;
}

void sys_sdcard_read(int addr, void * buf, int count)
{
	struct sdcard_info_t info;
	if(sdcard_info(&info))
		sdcard_helper_read(&info, addr, buf, count);
}
