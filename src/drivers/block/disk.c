/*
 * drivers/block/disk.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <block/disk.h>
#include <block/partition.h>

struct disk_block_t
{
	u64_t offset;
	struct disk_t * disk;
};

static u64_t disk_block_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(blk->priv);
	struct disk_t * disk = dblk->disk;
	return (disk->read(disk, buf, blkno + dblk->offset, blkcnt));
}

static u64_t disk_block_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(blk->priv);
	struct disk_t * disk = dblk->disk;
	return (disk->write(disk, buf, blkno + dblk->offset, blkcnt));
}

static void disk_block_sync(struct block_t * blk)
{
}

static void disk_block_suspend(struct block_t * blk)
{
}

static void disk_block_resume(struct block_t * blk)
{
}

static void disk_suspend(struct device_t * dev)
{
	struct disk_t * disk;

	if(!dev || dev->type != DEVICE_TYPE_DISK)
		return;

	disk = (struct disk_t *)(dev->driver);
	if(!disk)
		return;

	if(disk->sync)
		disk->sync(disk);

	if(disk->suspend)
		disk->suspend(disk);
}

static void disk_resume(struct device_t * dev)
{
	struct disk_t * disk;

	if(!dev || dev->type != DEVICE_TYPE_DISK)
		return;

	disk = (struct disk_t *)(dev->driver);
	if(!disk)
		return;

	if(disk->resume)
		disk->resume(disk);
}

static ssize_t partition_read_from(struct kobj_t * kobj, void * buf, size_t size)
{
	struct partition_t * part = (struct partition_t *)kobj->priv;
	return sprintf(buf, "%lld", part->from);
}

static ssize_t partition_read_to(struct kobj_t * kobj, void * buf, size_t size)
{
	struct partition_t * part = (struct partition_t *)kobj->priv;
	return sprintf(buf, "%lld", part->to);
}

static ssize_t partition_read_size(struct kobj_t * kobj, void * buf, size_t size)
{
	struct partition_t * part = (struct partition_t *)kobj->priv;
	return sprintf(buf, "%lld", part->size);
}

static ssize_t partition_read_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct partition_t * part = (struct partition_t *)kobj->priv;
	return sprintf(buf, "%lld", (part->to - part->from + 1) * part->size);
}

struct disk_t * search_disk(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_DISK);
	if(!dev)
		return NULL;

	return (struct disk_t *)dev->driver;
}

bool_t register_disk(struct disk_t * disk)
{
	struct device_t * dev;
	struct kobj_t * kobj;
	struct partition_t * ppos, * pn;
	struct block_t * blk;
	struct disk_block_t * dblk;
	char name[64];

	if(!disk)
		return FALSE;

	if(!disk->name || search_device(disk->name))
		return FALSE;

	if(!partition_map(disk))
		return FALSE;

	if(list_empty(&(disk->part.entry)))
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(disk->name);
	dev->type = DEVICE_TYPE_DISK;
	dev->suspend = disk_suspend;
	dev->resume = disk_resume;
	dev->driver = (void *)disk;
	dev->kobj = kobj_alloc_directory(dev->name);
	list_for_each_entry_safe(ppos, pn, &(disk->part.entry), entry)
	{
		kobj = kobj_search_directory_with_create(dev->kobj, ppos->name);
		kobj_add_regular(kobj, "from", partition_read_from, NULL, ppos);
		kobj_add_regular(kobj, "to", partition_read_to, NULL, ppos);
		kobj_add_regular(kobj, "size", partition_read_size, NULL, ppos);
		kobj_add_regular(kobj, "capacity", partition_read_capacity, NULL, ppos);
	}

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	list_for_each_entry_safe(ppos, pn, &(disk->part.entry), entry)
	{
		blk = malloc(sizeof(struct block_t));
		dblk = malloc(sizeof(struct disk_block_t));
		if(!blk || !dblk)
		{
			free(blk);
			free(dblk);
			unregister_disk(disk);
			return FALSE;
		}

		snprintf(name, sizeof(name), "%s.%s", disk->name, ppos->name);

		ppos->blk = blk;
		dblk->offset = ppos->from;
		dblk->disk = disk;

		blk->name = name;
		blk->blksz = ppos->size;
		blk->blkcnt = ppos->to - ppos->from + 1;
		blk->read = disk_block_read;
		blk->write = disk_block_write;
		blk->sync = disk_block_sync;
		blk->suspend = disk_block_suspend;
		blk->resume = disk_block_resume;
		blk->priv	= dblk;

		if(!register_block(blk))
		{
			free(blk);
			free(dblk);
			ppos->blk = NULL;
			unregister_disk(disk);
			return FALSE;
		}
	}

	return TRUE;
}

bool_t unregister_disk(struct disk_t * disk)
{
	struct device_t * dev;
	struct partition_t * ppos, * pn;
	struct block_t * blk;

	if(!disk || !disk->name)
		return FALSE;

	list_for_each_entry_safe(ppos, pn, &(disk->part.entry), entry)
	{
		blk = ppos->blk;
		if(blk)
		{
			unregister_block(blk);
			free(blk->priv);
			free(blk);
		}
		free(ppos);
	}

	dev = search_device_with_type(disk->name, DEVICE_TYPE_DISK);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

u64_t disk_read(struct disk_t * disk, u8_t * buf, u64_t offset, u64_t count)
{
	u64_t no, sz, cnt, capacity;
	u64_t len, tmp;
	u64_t ret = 0;
	u8_t * p;

	if(!disk || !buf || !count)
		return 0;

	sz = disk->size;
	cnt = disk->count;
	if(!sz || !cnt)
		return 0;

	capacity = sz * count;
	if(offset >= capacity)
		return 0;

	tmp = capacity - offset;
	if(count > tmp)
		count = tmp;

	p = malloc(sz);
	if(!p)
		return 0;

	no = offset / sz;
	tmp = offset % sz;
	if(tmp > 0)
	{
		len = sz - tmp;
		if(count < len)
			len = count;

		if(disk->read(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)buf, (const void *)(&p[tmp]), len);
		buf += len;
		count -= len;
		ret += len;
		no += 1;
	}

	tmp = count / sz;
	if(tmp > 0)
	{
		len = tmp * sz;

		if(disk->read(disk, buf, no, tmp) != tmp)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		no += tmp;
	}

	if(count > 0)
	{
		len = count;

		if(disk->read(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)buf, (const void *)(&p[0]), len);
		ret += len;
	}

	free(p);
	return ret;
}

u64_t disk_write(struct disk_t * disk, u8_t * buf, u64_t offset, u64_t count)
{
	u64_t no, sz, cnt, capacity;
	u64_t len, tmp;
	u64_t ret = 0;
	u8_t * p;

	if(!disk || !buf || !count)
		return 0;

	sz = disk->size;
	cnt = disk->count;
	if(!sz || !cnt)
		return 0;

	capacity = sz * count;
	if(offset >= capacity)
		return 0;

	tmp = capacity - offset;
	if(count > tmp)
		count = tmp;

	p = malloc(sz);
	if(!p)
		return 0;

	no = offset / sz;
	tmp = offset % sz;
	if(tmp > 0)
	{
		len = sz - tmp;
		if(count < len)
			len = count;

		if(disk->read(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)(&p[tmp]), (const void *)buf, len);

		if(disk->write(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		no += 1;
	}

	tmp = count / sz;
	if(tmp > 0)
	{
		len = tmp * sz;

		if(disk->write(disk, buf, no, tmp) != tmp)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		no += tmp;
	}

	if(count > 0)
	{
		len = count;

		if(disk->read(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)(&p[0]), (const void *)buf, len);

		if(disk->write(disk, p, no, 1) != 1)
		{
			free(p);
			return ret;
		}

		ret += len;
	}

	free(p);
	return ret;
}

void disk_sync(struct disk_t * disk)
{
	if(disk && disk->sync)
		disk->sync(disk);
}
