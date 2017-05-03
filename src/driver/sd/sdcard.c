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
	sdcard->rca = 0;
	return TRUE;
}

static bool_t sdcard_probe(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;
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

 	cmd.cmdidx = MMC_SELECT_CARD;
 	cmd.cmdarg = sdcard->rca << 16;
 	cmd.resptype = MMC_RSP_R1;
 	if(!sdhci_transfer(sdhci, &cmd, NULL))
 		return FALSE;

	return TRUE;
}

void sdcard_test(void)
{
	struct sdhci_t * sdhci = search_sdhci("sdhci-pl180.0");
	struct sdcard_t sdcard;

	memset(&sdcard, 0, sizeof(struct sdcard_t));
	printf("r = %d\r\n", sdcard_probe(sdhci, &sdcard));
}
