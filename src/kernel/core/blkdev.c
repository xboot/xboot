/*
 * kernel/core/blkdev.c
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
#include <xboot.h>
#include <malloc.h>
#include <div64.h>
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>

extern struct device_list * device_list;

/*
 * search block device by name
 */
struct blkdev * search_blkdev(const char * name)
{
	struct blkdev * dev;
	struct device_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == BLOCK_DEVICE)
		{
			dev = (struct blkdev *)(list->device->priv);
			if(strcmp((x_s8*)dev->name, (const x_s8 *)name) == 0)
				return dev;
		}
	}

	return NULL;
}

/*
 * search block device by name and block device type
 */
struct blkdev * search_blkdev_with_type(const char * name, enum blkdev_type type)
{
	struct blkdev * dev;
	struct device_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == BLOCK_DEVICE)
		{
			dev = (struct blkdev *)(list->device->priv);
			if(dev->type == type)
			{
				if(strcmp((x_s8*)dev->name, (const x_s8 *)name) == 0)
					return dev;
			}
		}
	}

	return NULL;
}

/*
 * register a block device into device_list
 */
x_bool register_blkdev(struct blkdev * dev)
{
	struct device * device;

	device = malloc(sizeof(struct device));
	if(!device || !dev)
	{
		free(device);
		return FALSE;
	}

	if(!dev->name || !dev->info || search_device(dev->name))
	{
		free(device);
		return FALSE;
	}

	device->name = dev->name;
	device->type = BLOCK_DEVICE;
	device->priv = (void *)dev;

	return register_device(device);
}

/*
 * unregister block device from blkdev_list
 */
x_bool unregister_blkdev(const char * name)
{
	struct device * device;

	if(!name)
		return FALSE;

	device = search_device(name);
	if(!device && device->type == BLOCK_DEVICE)
		return FALSE;

	if(unregister_device(device))
	{
		free(device);
		return TRUE;
	}

	return FALSE;
}

/*
 * get block device's total size
 */
x_size get_blkdev_total_size(struct blkdev * dev)
{
	struct blkinfo * list;
	struct list_head * pos;
	x_size size = 0;

	if(!dev || !dev->info)
		return 0;

	for(pos = (&(dev->info->entry))->next; pos != &(dev->info->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkinfo, entry);
		size += list->size * list->number;
	}

	return size;
}

/*
 * get block device's total number of block
 */
x_size get_blkdev_total_number(struct blkdev * dev)
{
	struct blkinfo * list;
	struct list_head * pos;
	x_size number = 0;

	if(!dev || !dev->info)
		return 0;

	for(pos = (&(dev->info->entry))->next; pos != &(dev->info->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkinfo, entry);
		number += list->number;
	}

	return number;
}

/*
 * get block device's block size by blkno
 */
x_s32 get_blkdev_size(struct blkdev * dev, x_s32 blkno)
{
	struct blkinfo * list;
	struct list_head * pos;

	if(!dev || !dev->info || blkno < 0)
		return 0;

	for(pos = (&(dev->info->entry))->next; pos != &(dev->info->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkinfo, entry);

		if((blkno >= list->blkno) && (blkno < (list->blkno + list->number)))
			return list->size;
		else
			continue;
	}

	return 0;
}

/*
 * get block device's offset by blkno
 */
x_size get_blkdev_offset(struct blkdev * dev, x_s32 blkno)
{
	struct blkinfo * list;
	struct list_head * pos;

	if(!dev || !dev->info || blkno < 0)
		return -1;

	for(pos = (&(dev->info->entry))->next; pos != &(dev->info->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkinfo, entry);

		if((blkno >= list->blkno) && (blkno < (list->blkno + list->number)))
			return ((x_size)(list->offset + (blkno - list->blkno) * list->size));
		else
			continue;
	}

	return -1;
}

/*
 * get block no by offset
 */
x_s32 get_blkdev_blkno(struct blkdev * dev, x_size offset)
{
	struct blkinfo * list;
	struct list_head * pos;

	if(!dev || !dev->info || offset < 0)
		return -1;

	for(pos = (&(dev->info->entry))->next; pos != &(dev->info->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkinfo, entry);

		if((offset >= list->offset) && (offset < (list->offset + list->size * list->number)))
			return (list->blkno + div64((offset - list->offset), list->size));
		else
			continue;
	}

	return -1;
}
