/*
 * driver/block/block.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

struct sub_block_pdata_t
{
	u64_t offset;
	u64_t length;
	struct block_t * pblk;
};

static ssize_t block_read_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct block_t * blk = (struct block_t *)kobj->priv;
	return sprintf(buf, "%lld", block_capacity(blk));
}

static u64_t sub_block_capacity(struct block_t * blk)
{
	struct sub_block_pdata_t * pdat = (struct sub_block_pdata_t *)(blk->priv);
	return pdat->length;
}

static u64_t sub_block_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	struct sub_block_pdata_t * pdat = (struct sub_block_pdata_t *)(blk->priv);
	struct block_t * pblk = pdat->pblk;
	return (pblk->read(pblk, buf, offset + pdat->offset, count));
}

static u64_t sub_block_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	struct sub_block_pdata_t * pdat = (struct sub_block_pdata_t *)(blk->priv);
	struct block_t * pblk = pdat->pblk;
	return (pblk->write(pblk, buf, offset + pdat->offset, count));
}

static void sub_block_sync(struct block_t * blk)
{
	struct sub_block_pdata_t * pdat = (struct sub_block_pdata_t *)(blk->priv);
	struct block_t * pblk = pdat->pblk;
	pblk->sync(pblk);
}

struct block_t * search_block(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_BLOCK);
	if(!dev)
		return NULL;
	return (struct block_t *)dev->priv;
}

struct device_t * register_block(struct block_t * blk, struct driver_t * drv)
{
	struct device_t * dev;

	if(!blk || !blk->name)
		return NULL;

	if(!blk->read || !blk->write || !blk->sync)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(blk->name);
	dev->type = DEVICE_TYPE_BLOCK;
	dev->driver = drv;
	dev->priv = blk;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "capacity", block_read_capacity, NULL, blk);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_block(struct block_t * blk)
{
	struct device_t * dev;

	if(blk && blk->name)
	{
		dev = search_device(blk->name, DEVICE_TYPE_BLOCK);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

struct device_t * register_sub_block(struct block_t * pblk, u64_t offset, u64_t length, const char * name)
{
	struct device_t * dev;
	struct block_t * blk;
	struct sub_block_pdata_t * pdat;
	char buffer[256];
	u64_t l;

	if(!name)
		return NULL;

	if(!pblk || !pblk->name)
		return NULL;

	l = block_available(pblk, offset, length);
	if(l <= 0)
		return NULL;

	blk = malloc(sizeof(struct block_t));
	pdat = malloc(sizeof(struct sub_block_pdata_t));
	if(!blk || !pdat)
	{
		free(blk);
		free(pdat);
		return NULL;
	}

	snprintf(buffer, sizeof(buffer), "%s.%s", pblk->name, name);
	pdat->offset = offset;
	pdat->length = length;
	pdat->pblk = pblk;

	blk->name = strdup(buffer);
	blk->capacity = sub_block_capacity;
	blk->read = sub_block_read;
	blk->write = sub_block_write;
	blk->sync = sub_block_sync;
	blk->priv = pdat;

	if(!(dev = register_block(blk, NULL)))
	{
		free(blk->priv);
		free(blk->name);
		free(blk);
		return NULL;
	}
	return dev;
}

void unregister_sub_block(struct block_t * pblk)
{
	struct device_t * pos, * n;
	struct block_t * blk;
	int len;

	if(pblk && pblk->name && search_block(pblk->name))
	{
		len = strlen(pblk->name);
		list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BLOCK], head)
		{
			if((strncmp(pos->name, pblk->name, len) == 0) && (strlen(pos->name) > len))
			{
				blk = (struct block_t *)pos->priv;
				if(blk)
				{
					unregister_block(blk);
					free(blk->priv);
					free(blk->name);
					free(blk);
				}
			}
		}
	}
}

u64_t block_capacity(struct block_t * blk)
{
	if(blk && blk->capacity)
		return blk->capacity(blk);
	return 0;
}

u64_t block_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	if(blk && blk->read)
		return blk->read(blk, buf, offset, count);
	return 0;
}

u64_t block_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	if(blk && blk->write)
		return blk->write(blk, buf, offset, count);
	return 0;
}

void block_sync(struct block_t * blk)
{
	if(blk && blk->sync)
		blk->sync(blk);
}
