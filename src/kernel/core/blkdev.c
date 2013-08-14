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
#include <xboot/blkdev.h>

static void blkdev_suspend(struct device_t * dev)
{
	struct blkdev_t * blk;

	if(!dev)
		return;

	if(dev->type != DEVICE_TYPE_BLOCK)
		return;

	blk = (struct blkdev_t *)(dev->driver);
	if(!blk)
		return;

	switch(blk->type)
	{
	default:
		break;
	}
}

static void blkdev_resume(struct device_t * dev)
{
	struct blkdev_t * blk;

	if(!dev)
		return;

	if(dev->type != DEVICE_TYPE_BLOCK)
		return;

	blk = (struct blkdev_t *)(dev->driver);
	if(!blk)
		return;

	switch(blk->type)
	{
	default:
		break;
	}
}

struct blkdev_t * search_blkdev(const char * name)
{
	struct device_list_t * pos, * n;
	struct blkdev_t * dev;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_BLOCK)
		{
			dev = (struct blkdev_t *)(pos->device->driver);
			if(strcmp(dev->name, name) == 0)
				return dev;
		}
	}

	return NULL;
}

struct blkdev_t * search_blkdev_with_type(const char * name, enum blkdev_type_t type)
{
	struct device_list_t * pos, * n;
	struct blkdev_t * dev;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_BLOCK)
		{
			dev = (struct blkdev_t *)(pos->device->driver);
			if(dev->type == type)
			{
				if(strcmp(dev->name, name) == 0)
					return dev;
			}
		}
	}

	return NULL;
}

bool_t register_blkdev(struct blkdev_t * dev)
{
	struct device_t * device;

	if(!dev)
		return FALSE;

	if(!dev->name || search_device(dev->name))
		return FALSE;

	device = malloc(sizeof(struct device_t));
	if(!device)
		return FALSE;

	device->name = strdup(dev->name);
	device->type = DEVICE_TYPE_BLOCK;
	device->suspend = blkdev_suspend;
	device->resume = blkdev_resume;
	device->driver = (void *)dev;

	return register_device(device);
}

bool_t unregister_blkdev(const char * name)
{
	struct device_t * device;

	if(!name)
		return FALSE;

	device = search_device(name);
	if(!device && device->type == DEVICE_TYPE_BLOCK)
		return FALSE;

	if(unregister_device(device))
	{
		free(device->name);
		free(device);
		return TRUE;
	}

	return FALSE;
}

loff_t get_blkdev_total_size(struct blkdev_t * dev)
{
	if(!dev)
		return 0;

	return (dev->blksz * dev->blkcnt);
}

size_t get_blkdev_total_count(struct blkdev_t * dev)
{
	if(!dev)
		return 0;

	return (dev->blkcnt);
}

size_t get_blkdev_size(struct blkdev_t * dev)
{
	if(!dev)
		return 0;

	return (dev->blksz);
}

loff_t get_blkdev_offset(struct blkdev_t * dev, size_t blkno)
{
	if(!dev)
		return -1;

	if(blkno > dev->blkcnt)
		return -1;

	return (dev->blksz * blkno);
}
