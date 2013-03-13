/*
 * arch/arm/mach-x6410/s3c6410-nfc.c
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
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-nand.h>


static void s3c6410_nfc_init(void)
{
	writel(S3C6410_GPPCON, 0x2aaaaaaa);
	writel(S3C6410_NFCONF, readl(S3C6410_NFCONF) | 0x00007770);
	writel(S3C6410_NFCONT, readl(S3C6410_NFCONT) & ~(1 << 16));
}

static void s3c6410_nfc_exit(void)
{
}

static bool_t s3c6410_nfc_control(struct nand_device * nand, enum nand_control ctl)
{
	switch(ctl)
	{
	case NAND_ENABLE_CONTROLLER:
		writel(S3C6410_NFCONT, readl(S3C6410_NFCONT) | (1 << 0));
		return TRUE;

	case NAND_DISABLE_CONTROLLER:
		writel(S3C6410_NFCONT, readl(S3C6410_NFCONT) & ~(1 << 0));
		return TRUE;

	case NAND_ENABLE_CE:
		writel(S3C6410_NFCONT, readl(S3C6410_NFCONT) & ~(1 << 1));
		return TRUE;

	case NAND_DISABLE_CE:
		writel(S3C6410_NFCONT, readl(S3C6410_NFCONT) | (1 << 1));
		return TRUE;

	default:
		return FALSE;
	}

	return FALSE;
}

static bool_t s3c6410_nfc_command(struct nand_device * nand, u32_t cmd)
{
	writel(S3C6410_NFCMD, cmd);

	return TRUE;
}

static bool_t s3c6410_nfc_address(struct nand_device * nand, u32_t addr)
{
	writel(S3C6410_NFADDR, addr);

	return TRUE;
}

static bool_t s3c6410_nfc_read_data(struct nand_device * nand, u32_t * data)
{
	*data = readb(S3C6410_NFDATA);

	return TRUE;
}

static bool_t s3c6410_nfc_write_data(struct nand_device * nand, u32_t data)
{
	writeb(S3C6410_NFDATA, data);

	return TRUE;
}

static bool_t s3c6410_nfc_nand_ready(struct nand_device * nand, s32_t timeout)
{
	u32_t status;

	do {
		status = readl(S3C6410_NFSTAT);
		if(status & 0x01)
			return TRUE;
		mdelay(1);
	} while(timeout-- > 0);

	return FALSE;
}

static struct nfc s3c6410_nand_flash_controller = {
	.name				= "s3c6410-nfc",
	.init				= s3c6410_nfc_init,
	.exit				= s3c6410_nfc_exit,
	.control			= s3c6410_nfc_control,
	.command			= s3c6410_nfc_command,
	.address			= s3c6410_nfc_address,
	.read_data			= s3c6410_nfc_read_data,
	.write_data			= s3c6410_nfc_write_data,
	.nand_ready			= s3c6410_nfc_nand_ready,
};

static __init void s3c6410_nand_flash_controller_init(void)
{
	if(!register_nfc(&s3c6410_nand_flash_controller))
		LOG_E("failed to register nand flash controller '%s'", s3c6410_nand_flash_controller.name);
}

static __exit void s3c6410_nand_flash_controller_exit(void)
{
	if(!unregister_nfc(&s3c6410_nand_flash_controller))
		LOG_E("failed to unregister nand flash controller '%s'", s3c6410_nand_flash_controller.name);
}

core_initcall(s3c6410_nand_flash_controller_init);
core_exitcall(s3c6410_nand_flash_controller_init);
