/*
 * driver/block/romdisk/romdisk.c
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

#include <xboot.h>
#include <block/block.h>

struct romdisk_pdata_t
{
	virtual_addr_t addr;
	virtual_size_t size;
};

static u64_t romdisk_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct romdisk_pdata_t * pdat = (struct romdisk_pdata_t *)(blk->priv);
	virtual_addr_t offset = pdat->addr + block_offset(blk, blkno);
	u64_t count = block_available_count(blk, blkno, blkcnt);
	u64_t length = block_available_length(blk, blkno, blkcnt);

	memcpy((void *)buf, (const void *)(offset), length);
	return count;
}

static u64_t romdisk_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

static void romdisk_sync(struct block_t * blk)
{
}

static struct device_t * romdisk_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct romdisk_pdata_t * pdat;
	struct block_t * blk;
	struct device_t * dev;
	virtual_addr_t addr = strtoull(dt_read_string(n, "address", "0"), NULL, 0);
	virtual_size_t size = strtoull(dt_read_string(n, "size", "0"), NULL, 0);

	if(size <= 0)
		return NULL;
	size = (size + SZ_512) / SZ_512;

	pdat = malloc(sizeof(struct romdisk_pdata_t));
	if(!pdat)
		return NULL;

	blk = malloc(sizeof(struct block_t));
	if(!blk)
	{
		free(pdat);
		return NULL;
	}

	pdat->addr = addr;
	pdat->size = size;

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	blk->blksz	= SZ_512;
	blk->blkcnt	= (u64_t)size;
	blk->read = romdisk_read;
	blk->write = romdisk_write;
	blk->sync = romdisk_sync;
	blk->priv = pdat;

	if(!register_block(&dev, blk))
	{
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void romdisk_remove(struct device_t * dev)
{
	struct block_t * blk = (struct block_t *)dev->priv;

	if(blk && unregister_block(blk))
	{
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
	}
}

static void romdisk_suspend(struct device_t * dev)
{
}

static void romdisk_resume(struct device_t * dev)
{
}

static struct driver_t romdisk = {
	.name		= "romdisk",
	.probe		= romdisk_probe,
	.remove		= romdisk_remove,
	.suspend	= romdisk_suspend,
	.resume		= romdisk_resume,
};

static __init void romdisk_driver_init(void)
{
	register_driver(&romdisk);
}

static __exit void romdisk_driver_exit(void)
{
	unregister_driver(&romdisk);
}

driver_initcall(romdisk_driver_init);
driver_exitcall(romdisk_driver_exit);
