/*
 * brom-copyself.c
 *
 * This file used by start.s assembler code, and the linker script
 * must make sure this file is linked within the first 32kB. DO NOT
 * use any .bss segment and .data segment, Just use brom's small
 * stack. It's just a small c routline.
 *
 * NOTE:
 *     Please do not use 'switch, case' instead of 'if, else if',
 *     Because switch statement may be compiled as a jump table,
 *     Which belong to .data segment, If it has many cases.
 *
 *     To make sure this file is linked within the first 32KB, please
 *     look at the file of xboot.map.
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

extern void return_to_fel(void);

enum {
	BOOT_DEVICE_FEL	= 0,
	BOOT_DEVICE_MMC	= 1,
	BOOT_DEVICE_SPI	= 2,
};

static int get_boot_device(void)
{
	u32_t * sig = (void *)0x4;
	u32_t d = sig[9] & 0xff;

	if((sig[0] == 0x4e4f4765) && (sig[1] == 0x3054422e))
	{
		if(d == 0)
			return BOOT_DEVICE_MMC;
		else if(d == 3)
			return BOOT_DEVICE_SPI;
	}
	return BOOT_DEVICE_FEL;
}

void brom_copyself(void)
{
	int d = get_boot_device();

	if(d == BOOT_DEVICE_FEL)
	{
		return_to_fel();
	}
	else if(d == BOOT_DEVICE_MMC)
	{
	}
	else if(d == BOOT_DEVICE_SPI)
	{
	}
}
