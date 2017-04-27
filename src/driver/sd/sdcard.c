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

	if(sdhci_request(sdhci, &cmd, NULL))
		return TRUE;
	udelay(2000);
	return sdhci_request(sdhci, &cmd, NULL);
}

static bool_t mmc_send_op_cond(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;
	int timeout = 10;

	if(!mmc_go_idle(sdhci))
		return FALSE;

 	cmd.cmdidx = MMC_SEND_OP_COND;
 	cmd.cmdarg = 0;
 	cmd.resptype = MMC_RSP_R3;
 	if(!sdhci_request(sdhci, &cmd, NULL))
 		return FALSE;

	do {
		udelay(1000);

		cmd.cmdidx = MMC_SEND_OP_COND;
		cmd.cmdarg = MMC_VDD_33_34;
		cmd.resptype = MMC_RSP_R3;
		cmd.response[0] = 0;
	 	if(!sdhci_request(sdhci, &cmd, NULL))
	 		return FALSE;
	} while (!(cmd.response[0] & 0x80000000) && timeout--);

	if(timeout <= 0)
		return FALSE;

	sdcard->version = MMC_VERSION_UNKNOWN;
	sdcard->ocr = cmd.response[0];
	sdcard->rca = 0;
	return TRUE;
}

static bool_t mmc_send_if_cond(struct sdhci_t * sdhci, struct sdcard_t * sdcard)
{
	struct sdhci_cmd_t cmd;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	cmd.cmdarg = MMC_VDD_33_34;
	cmd.resptype = MMC_RSP_R7;
	cmd.response[0] = 0;
 	if(!sdhci_request(sdhci, &cmd, NULL))
 		return FALSE;

 	if((cmd.response[0] & 0xff) == 0xaa)
		sdcard->version = SD_VERSION_2;
	return TRUE;
}

void sdcard_test(void)
{
	struct sdhci_t * sdhci = search_sdhci("sdhci-pl180.0");
	bool_t ret;

	ret = mmc_go_idle(sdhci);
	printf("r = %d\r\n", ret);
}
