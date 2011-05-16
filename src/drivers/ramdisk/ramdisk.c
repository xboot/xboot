/*
 * drivers/ramdisk/ramdisk.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

extern u8_t __ramdisk_start[];
extern u8_t __ramdisk_end[];

/*
 * the struct of ramdisk
 */
struct ramdisk
{
	/* the ramdisk name */
	char name[32 + 1];

	/* the start of ramdisk */
	void * start;

	/* the end of ramdisk */
	void * end;

	/* busy or not */
	bool_t busy;
};

static s32_t ramdisk_open(struct blkdev * dev)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);

	if(ramdisk->busy == TRUE)
		return -1;

	ramdisk->busy = TRUE;
	return 0;
}

static s32_t ramdisk_read(struct blkdev * dev, u8_t * buf, u32_t blkno, u32_t blkcnt)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);
	u8_t * p = (u8_t *)(ramdisk->start);
	loff_t offset = get_blkdev_offset(dev, blkno);
	s32_t size = get_blkdev_size(dev) * blkcnt;

	if(offset < 0)
		return 0;

	if(size < 0)
		return 0;

	memcpy((void *)buf, (const void *)(p + offset), size);
	return blkcnt;
}

static s32_t ramdisk_write(struct blkdev * dev, const u8_t * buf, u32_t blkno, u32_t blkcnt)
{
	return 0;
}

static s32_t ramdisk_ioctl(struct blkdev * dev, u32_t cmd, void * arg)
{
	return -1;
}

static s32_t ramdisk_close(struct blkdev * dev)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);

	ramdisk->busy = FALSE;
	return 0;
}

static __init void ramdisk_init(void)
{
	struct blkdev * dev;
	struct ramdisk * ramdisk;
	u64_t size, rem;

	dev = malloc(sizeof(struct blkdev));
	if(!dev)
		return;

	ramdisk = malloc(sizeof(struct ramdisk));
	if(!ramdisk)
	{
		free(dev);
		return;
	}

	snprintf(ramdisk->name, 32, "ramdisk");
	ramdisk->start = (void *)__ramdisk_start;
	ramdisk->end = (void *)__ramdisk_end;
	ramdisk->busy = FALSE;

	if((ramdisk->end - ramdisk->start) <= 0)
	{
		free(ramdisk);
		free(dev);
		return;
	}

	size = (u64_t)(ramdisk->end - ramdisk->start);
	rem = div64_64(&size, SZ_512);
	if(rem > 0)
		size++;

	ramdisk->busy	= FALSE;

	dev->name		= ramdisk->name;
	dev->type		= BLK_DEV_RAMDISK;
	dev->blksz		= SZ_512;
	dev->blkcnt		= size;
	dev->open 		= ramdisk_open;
	dev->read 		= ramdisk_read;
	dev->write		= ramdisk_write;
	dev->ioctl 		= ramdisk_ioctl;
	dev->close		= ramdisk_close;
	dev->driver	 = ramdisk;

	if(!register_blkdev(dev))
	{
		free(ramdisk);
		free(dev);
		return;
	}
}

static __exit void ramdisk_exit(void)
{
	struct blkdev * dev;
	struct ramdisk * ramdisk;

	dev = search_blkdev_with_type("ramdisk", BLK_DEV_RAMDISK);
	if(dev)
	{
		ramdisk = (struct ramdisk *)(dev->driver);

		if(unregister_blkdev(dev->name))
		{
			free(ramdisk);
			free(dev);
			return;
		}
	}
}

module_init(ramdisk_init, LEVEL_DRIVER);
module_exit(ramdisk_exit, LEVEL_DRIVER);
