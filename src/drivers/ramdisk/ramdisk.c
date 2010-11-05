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

extern x_u8 __ramdisk_start[];
extern x_u8 __ramdisk_end[];

/*
 * the struct of ramdisk
 */
struct ramdisk
{
	/* the ramdisk name */
	char name[32 + 1];

	/* block information */
	struct blkinfo info;

	/* the start of ramdisk */
	x_sys start;

	/* the end of ramdisk */
	x_sys end;

	/* busy or not */
	x_bool busy;
};

static x_s32 ramdisk_open(struct blkdev * dev)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);

	if(ramdisk->busy == TRUE)
		return -1;

	ramdisk->busy = TRUE;
	return 0;
}

static x_s32 ramdisk_read(struct blkdev * dev, x_u8 * buf, x_s32 blkno)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);
	x_u8 * p = (x_u8 *)(ramdisk->start);
	x_size offset;
	x_s32 size;

	if(blkno < 0)
		return 0;

	offset = get_blkdev_offset(dev, blkno);
	if(offset < 0)
		return 0;

	size = get_blkdev_size(dev, blkno);

	memcpy((void *)buf, (const void *)(p + offset), size);
	return size;
}

static x_s32 ramdisk_write(struct blkdev * dev, const x_u8 * buf, x_s32 blkno)
{
	return 0;
}

static x_s32 ramdisk_ioctl(struct blkdev * dev, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 ramdisk_close(struct blkdev * dev)
{
	struct ramdisk * ramdisk = (struct ramdisk *)(dev->driver);

	ramdisk->busy = FALSE;
	return 0;
}

static __init void ramdisk_init(void)
{
	struct blkdev * dev;
	struct ramdisk * ramdisk;
	struct blkinfo * info;
	struct list_head * info_pos;
	x_u64 size, rem;

	dev = malloc(sizeof(struct blkdev));
	if(!dev)
		return;

	ramdisk = malloc(sizeof(struct ramdisk));
	if(!ramdisk)
	{
		free(dev);
		return;
	}

	snprintf((x_s8 *)ramdisk->name, 32, (const x_s8 *)"ramdisk");
	ramdisk->start = (x_sys)__ramdisk_start;
	ramdisk->end = (x_sys)__ramdisk_end;
	ramdisk->busy = FALSE;

	if((ramdisk->end - ramdisk->start) <= 0)
	{
		free(ramdisk);
		free(dev);
		return;
	}

	size = (x_u64)(ramdisk->end - ramdisk->start);
	rem = div64_64(&size, 512);

	init_list_head(&(ramdisk->info.entry));

	if(size > 0)
	{
		info = malloc(sizeof(struct blkinfo));
		if(!info)
		{
			free(ramdisk);
			free(dev);
			return;
		}

		info->blkno = 0;
		info->offset = 0;
		info->size = 512;
		info->number = size;
		list_add_tail(&info->entry, &(ramdisk->info.entry));
	}

	if(rem > 0)
	{
		info = malloc(sizeof(struct blkinfo));
		if(!info)
		{
			for(info_pos = (&(ramdisk->info.entry))->next; info_pos != &(ramdisk->info.entry); info_pos = info_pos->next)
			{
				info = list_entry(info_pos, struct blkinfo, entry);
				free(info);
			}
			free(ramdisk);
			free(dev);
			return;
		}

		info->blkno = size;
		info->offset = size * 512;
		info->size = rem;
		info->number = 1;
		list_add_tail(&info->entry, &(ramdisk->info.entry));
	}

	dev->name	= ramdisk->name;
	dev->type	= BLK_DEV_RAMDISK;
	dev->info	= &(ramdisk->info);
	dev->open 	= ramdisk_open;
	dev->read 	= ramdisk_read;
	dev->write	= ramdisk_write;
	dev->ioctl 	= ramdisk_ioctl;
	dev->close	= ramdisk_close;
	dev->driver = ramdisk;

	if(!register_blkdev(dev))
	{
		for(info_pos = (&(ramdisk->info.entry))->next; info_pos != &(ramdisk->info.entry); info_pos = info_pos->next)
		{
			info = list_entry(info_pos, struct blkinfo, entry);
			free(info);
		}
		free(ramdisk);
		free(dev);
		return;
	}
}

static __exit void ramdisk_exit(void)
{
	struct blkdev * dev;
	struct ramdisk * ramdisk;
	struct blkinfo * info;
	struct list_head * info_pos;

	dev = search_blkdev_with_type("ramdisk", BLK_DEV_RAMDISK);
	if(dev)
	{
		ramdisk = (struct ramdisk *)(dev->driver);

		if(unregister_blkdev(dev->name))
		{
			for(info_pos = (&(ramdisk->info.entry))->next; info_pos != &(ramdisk->info.entry); info_pos = info_pos->next)
			{
				info = list_entry(info_pos, struct blkinfo, entry);
				free(info);
			}
			free(ramdisk);
			free(dev);
			return;
		}
	}
}

module_init(ramdisk_init, LEVEL_DRIVER);
module_exit(ramdisk_exit, LEVEL_DRIVER);
