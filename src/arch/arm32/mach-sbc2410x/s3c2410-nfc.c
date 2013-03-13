/*
 * arch/arm/mach-sbc2410x/s3c2410-nfc.c
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
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <time/delay.h>
#include <mtd/nand/nfc.h>
#include <s3c2410/reg-nand.h>

#if 0
void s3c2410_nfc_init(struct nand_chip * chip)
{
	writel(S3C2410_NFCONF, S3C2410_NFCONF_EN | S3C2410_NFCONF_TACLS(3) |S3C2410_NFCONF_TWRPH0(5) | S3C2410_NFCONF_TWRPH1(3));
}

void s3c2410_nfc_exit(struct nand_chip * chip)
{
}

void s3c2410_nfc_reset(struct nand_chip * chip)
{
	writel(S3C2410_NFCMD, 0xff);
}

bool_t s3c2410_nfc_command(struct nand_chip * chip, u32_t command)
{
	writew(S3C2410_NFCMD, command);
	return TRUE;
}

bool_t s3c2410_nfc_address(struct nand_chip * chip, u32_t address)
{
	writew(S3C2410_NFADDR, address);
	return TRUE;
}

bool_t s3c2410_nfc_write_data(struct nand_chip * chip, u32_t data)
{
	writeb(S3C2410_NFDATA, data);
	return TRUE;
}

bool_t s3c2410_nfc_read_data(struct nand_chip * chip, u32_t * data)
{
	*data = readb(S3C2410_NFDATA);
	return TRUE;
}

bool_t s3c2410_nfc_controller_ready(struct nand_chip * chip, s32_t timeout)
{
	return TRUE;
}

bool_t s3c2410_nfc_nand_ready(struct nand_chip * chip, s32_t timeout)
{
	u8_t status;

	do {
		status = readb(S3C2410_NFSTAT);
		if(status & S3C2410_NFSTAT_BUSY)
			return TRUE;
		mdelay(1);
	}while(timeout-- > 0);

	return FALSE;
}

static struct nfc s3c2410_nand_flash_controller = {
	.name				= "s3c2410-nfc",
	.init				= s3c2410_nfc_init,
	.exit				= s3c2410_nfc_exit,
	.reset				= s3c2410_nfc_reset,
	.command			= s3c2410_nfc_command,
	.address			= s3c2410_nfc_address,
	.write_data			= s3c2410_nfc_write_data,
	.read_data			= s3c2410_nfc_read_data,
	.write_page			= 0,
	.read_page			= 0,
	.write_block_data	= 0,
	.read_block_data	= 0,
	.controller_ready	= s3c2410_nfc_controller_ready,
	.nand_ready			= s3c2410_nfc_nand_ready,
};

static __init void s3c2410_nand_flash_controller_init(void)
{
	if(!register_nfc(&s3c2410_nand_flash_controller))
		LOG_E("failed to register nand flash controller '%s'", s3c2410_nand_flash_controller.name);
}

static __exit void s3c2410_nand_flash_controller_exit(void)
{
	if(!unregister_nfc(&s3c2410_nand_flash_controller))
		LOG_E("failed to unregister nand flash controller '%s'", s3c2410_nand_flash_controller.name);
}

core_initcall(s3c2410_nand_flash_controller_init);
core_exitcall(s3c2410_nand_flash_controller_exit);

#endif

