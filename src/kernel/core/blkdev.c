/*
 * kernel/core/blkdev.c
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
#include <stddef.h>
#include <malloc.h>
#include <div64.h>
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
			if(strcmp((const char *)dev->name, (const char *)name) == 0)
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
				if(strcmp((const char *)dev->name, (const char *)name) == 0)
					return dev;
			}
		}
	}

	return NULL;
}

/*
 * register a block device into device_list
 */
bool_t register_blkdev(struct blkdev * dev)
{
	struct device * device;

	device = malloc(sizeof(struct device));
	if(!device || !dev)
	{
		free(device);
		return FALSE;
	}

	if(!dev->name || search_device(dev->name))
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
bool_t unregister_blkdev(const char * name)
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
loff_t get_blkdev_total_size(struct blkdev * dev)
{
	if(!dev)
		return 0;

	return (dev->blksz * dev->blkcnt);
}

/*
 * get block device's total count of block
 */
size_t get_blkdev_total_count(struct blkdev * dev)
{
	if(!dev)
		return 0;

	return (dev->blkcnt);
}

/*
 * get block device's block size
 */
size_t get_blkdev_size(struct blkdev * dev)
{
	if(!dev)
		return 0;

	return (dev->blksz);
}

/*
 * get block device's offset by blkno
 */
loff_t get_blkdev_offset(struct blkdev * dev, size_t blkno)
{
	if(!dev)
		return -1;

	if(blkno > dev->blkcnt)
		return -1;

	return (dev->blksz * blkno);
}
