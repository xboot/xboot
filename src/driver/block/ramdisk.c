/*
 * driver/block/ramdisk.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

struct ramdisk_pdata_t
{
	virtual_addr_t addr;
	virtual_size_t size;
};

static u64_t ramdisk_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct ramdisk_pdata_t * pdat = (struct ramdisk_pdata_t *)(blk->priv);
	virtual_addr_t offset = pdat->addr + block_offset(blk, blkno);
	u64_t length = block_size(blk) * blkcnt;
	memcpy((void *)buf, (const void *)(offset), length);
	return blkcnt;
}

static u64_t ramdisk_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct ramdisk_pdata_t * pdat = (struct ramdisk_pdata_t *)(blk->priv);
	virtual_addr_t offset = pdat->addr + block_offset(blk, blkno);
	u64_t length = block_size(blk) * blkcnt;
	memcpy((void *)(offset), (const void *)buf, length);
	return blkcnt;
}

static void ramdisk_sync(struct block_t * blk)
{
}

static struct device_t * ramdisk_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ramdisk_pdata_t * pdat;
	struct block_t * blk;
	struct device_t * dev;
	u64_t blkcnt, blksz = SZ_512;
	virtual_addr_t addr = dt_read_long(n, "address", 0);
	virtual_size_t size = dt_read_long(n, "size", 0);

	if(size < blksz)
		return NULL;
	blkcnt = size / blksz;

	pdat = malloc(sizeof(struct ramdisk_pdata_t));
	if(!pdat)
		return NULL;

	blk = malloc(sizeof(struct block_t));
	if(!blk)
	{
		free(pdat);
		return NULL;
	}

	pdat->addr = addr;
	pdat->size = blkcnt * blksz;

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	blk->blksz	= blksz;
	blk->blkcnt	= blkcnt;
	blk->read = ramdisk_read;
	blk->write = ramdisk_write;
	blk->sync = ramdisk_sync;
	blk->priv = pdat;

	if(!(dev = register_block(blk, drv)))
	{
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
		return NULL;
	}
	return dev;
}

static void ramdisk_remove(struct device_t * dev)
{
	struct block_t * blk = (struct block_t *)dev->priv;

	if(blk)
	{
		unregister_block(blk);
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
	}
}

static void ramdisk_suspend(struct device_t * dev)
{
}

static void ramdisk_resume(struct device_t * dev)
{
}

static struct driver_t ramdisk = {
	.name		= "ramdisk",
	.probe		= ramdisk_probe,
	.remove		= ramdisk_remove,
	.suspend	= ramdisk_suspend,
	.resume		= ramdisk_resume,
};

static __init void ramdisk_driver_init(void)
{
	register_driver(&ramdisk);
}

static __exit void ramdisk_driver_exit(void)
{
	unregister_driver(&ramdisk);
}

driver_initcall(ramdisk_driver_init);
driver_exitcall(ramdisk_driver_exit);
