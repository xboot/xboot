/*
 * drivers/ramdisk/ramdisk.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <div64.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>
#include <xboot/ioctl.h>

static __init void ramdisk_block_init(void)
{
	struct blkdev * dev;
	struct blkinfo * info;
	x_u64 size, rem;
	//TODO
	//XXX
	//FIXME
	return;
	dev = malloc(sizeof(struct blkdev));
	info = malloc(sizeof(struct blkinfo));

	if(!dev || !info)
	{
		free(dev);
		free(info);
		return;
	}
/*
	if(!register_blkdev(&ramdisk))
		LOG_E("failed to register block driver '%s'", ramdisk.name);
		*/
}

static __exit void ramdisk_block_exit(void)
{
	/*
	if(!unregister_blkdev(ramdisk.name))
		LOG_E("failed to unregister rtc driver '%s'", ramdisk.name);
		*/
}

module_init(ramdisk_block_init, LEVEL_DRIVER);
module_exit(ramdisk_block_exit, LEVEL_DRIVER);
