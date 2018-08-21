/*
 * driver/block/block.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <block/block.h>

static ssize_t block_read_size(struct kobj_t * kobj, void * buf, size_t size)
{
	struct block_t * blk = (struct block_t *)kobj->priv;
	return sprintf(buf, "%lld", block_size(blk));
}

static ssize_t block_read_count(struct kobj_t * kobj, void * buf, size_t size)
{
	struct block_t * blk = (struct block_t *)kobj->priv;
	return sprintf(buf, "%lld", block_count(blk));
}

static ssize_t block_read_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct block_t * blk = (struct block_t *)kobj->priv;
	return sprintf(buf, "%lld", block_capacity(blk));
}

struct block_t * search_block(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_BLOCK);
	if(!dev)
		return NULL;
	return (struct block_t *)dev->priv;
}

bool_t register_block(struct device_t ** device, struct block_t * blk)
{
	struct device_t * dev;

	if(!blk || !blk->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(blk->name);
	dev->type = DEVICE_TYPE_BLOCK;
	dev->priv = blk;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "size", block_read_size, NULL, blk);
	kobj_add_regular(dev->kobj, "count", block_read_count, NULL, blk);
	kobj_add_regular(dev->kobj, "capacity", block_read_capacity, NULL, blk);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_block(struct block_t * blk)
{
	struct device_t * dev;

	if(!blk || !blk->name)
		return FALSE;

	dev = search_device(blk->name, DEVICE_TYPE_BLOCK);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

u64_t block_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	u64_t blkno, blksz, blkcnt, capacity;
	u64_t len, tmp;
	u64_t ret = 0;
	u8_t * p;

	if(!blk || !buf || !count)
		return 0;

	blksz = block_size(blk);
	blkcnt = block_count(blk);
	if(!blksz || !blkcnt)
		return 0;

	capacity = block_capacity(blk);
	if(offset >= capacity)
		return 0;

	tmp = capacity - offset;
	if(count > tmp)
		count = tmp;

	p = malloc(blksz);
	if(!p)
		return 0;

	blkno = offset / blksz;
	tmp = offset % blksz;
	if(tmp > 0)
	{
		len = blksz - tmp;
		if(count < len)
			len = count;

		if(blk->read(blk, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)buf, (const void *)(&p[tmp]), len);
		buf += len;
		count -= len;
		ret += len;
		blkno += 1;
	}

	tmp = count / blksz;
	if(tmp > 0)
	{
		len = tmp * blksz;

		if(blk->read(blk, buf, blkno, tmp) != tmp)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		blkno += tmp;
	}

	if(count > 0)
	{
		len = count;

		if(blk->read(blk, p, blkno, 1) != 1)
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

u64_t block_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	u64_t blkno, blksz, blkcnt, capacity;
	u64_t len, tmp;
	u64_t ret = 0;
	u8_t * p;

	if(!blk || !buf || !count)
		return 0;

	blksz = block_size(blk);
	blkcnt = block_count(blk);
	if(!blksz || !blkcnt)
		return 0;

	capacity = block_capacity(blk);
	if(offset >= capacity)
		return 0;

	tmp = capacity - offset;
	if(count > tmp)
		count = tmp;

	p = malloc(blksz);
	if(!p)
		return 0;

	blkno = offset / blksz;
	tmp = offset % blksz;
	if(tmp > 0)
	{
		len = blksz - tmp;
		if(count < len)
			len = count;

		if(blk->read(blk, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)(&p[tmp]), (const void *)buf, len);

		if(blk->write(blk, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		blkno += 1;
	}

	tmp = count / blksz;
	if(tmp > 0)
	{
		len = tmp * blksz;

		if(blk->write(blk, buf, blkno, tmp) != tmp)
		{
			free(p);
			return ret;
		}

		buf += len;
		count -= len;
		ret += len;
		blkno += tmp;
	}

	if(count > 0)
	{
		len = count;

		if(blk->read(blk, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}

		memcpy((void *)(&p[0]), (const void *)buf, len);

		if(blk->write(blk, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}

		ret += len;
	}

	free(p);
	return ret;
}

void block_sync(struct block_t * blk)
{
	if(blk && blk->sync)
		blk->sync(blk);
}
