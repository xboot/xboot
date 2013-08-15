/*
 * drivers/block/block.c
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

#include <block/block.h>

static void block_suspend(struct device_t * dev)
{
	struct block_t * blk;

	if(!dev || dev->type != DEVICE_TYPE_BLOCK)
		return;

	blk = (struct block_t *)(dev->driver);
	if(!blk)
		return;
}

static void block_resume(struct device_t * dev)
{
	struct block_t * blk;

	if(!dev || dev->type != DEVICE_TYPE_BLOCK)
		return;

	blk = (struct block_t *)(dev->driver);
	if(!blk)
		return;
}

struct block_t * search_block(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_BLOCK);
	if(!dev)
		return NULL;

	return (struct block_t *)dev->driver;
}

bool_t register_block(struct block_t * blk)
{
	struct device_t * device;

	if(!blk)
		return FALSE;

	if(!blk->name || search_device(blk->name))
		return FALSE;

	device = malloc(sizeof(struct device_t));
	if(!device)
		return FALSE;

	device->name = strdup(blk->name);
	device->type = DEVICE_TYPE_BLOCK;
	device->suspend = block_suspend;
	device->resume = block_resume;
	device->driver = (void *)blk;

	return register_device(device);
}

bool_t unregister_block(const char * name)
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

loff_t get_block_total_size(struct block_t * blk)
{
	if(!blk)
		return 0;

	return (blk->blksz * blk->blkcnt);
}

size_t get_block_total_count(struct block_t * blk)
{
	if(!blk)
		return 0;

	return (blk->blkcnt);
}

size_t get_block_size(struct block_t * blk)
{
	if(!blk)
		return 0;

	return (blk->blksz);
}

loff_t get_block_offset(struct block_t * blk, size_t blkno)
{
	if(!blk)
		return -1;

	if(blkno > blk->blkcnt)
		return -1;

	return (blk->blksz * blkno);
}
