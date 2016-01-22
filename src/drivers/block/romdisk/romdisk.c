/*
 * drivers/block/romdisk/romdisk.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <block/block.h>

extern u8_t __romdisk_start[];
extern u8_t __romdisk_end[];

struct romdisk_t
{
	char * name;
	void * start;
	void * end;
};

static u64_t romdisk_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(blk->priv);
	u64_t offset = block_offset(blk, blkno);
	u64_t length = block_available_length(blk, blkno, blkcnt);
	u64_t count = block_available_count(blk, blkno, blkcnt);

	memcpy((void *)buf, (const void *)((u8_t *)(romdisk->start) + offset), length);
	return count;
}

static u64_t romdisk_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

static void romdisk_sync(struct block_t * blk)
{
}

static void romdisk_suspend(struct block_t * blk)
{
}

static void romdisk_resume(struct block_t * blk)
{
}

static bool_t register_romdisk(const char * name, void * start, void * end)
{
	struct block_t * blk;
	struct romdisk_t * romdisk;
	u64_t size;

	if(!name)
		return FALSE;

	size = (u64_t)(end - start);
	size = (size + SZ_512) / SZ_512;
	if(size <= 0)
		return FALSE;

	blk = malloc(sizeof(struct block_t));
	if(!blk)
		return FALSE;

	romdisk = malloc(sizeof(struct romdisk_t));
	if(!romdisk)
	{
		free(blk);
		return FALSE;
	}

	romdisk->name = strdup(name);
	romdisk->start = start;
	romdisk->end = end;

	blk->name	= romdisk->name;
	blk->blksz	= SZ_512;
	blk->blkcnt	= size;
	blk->read 	= romdisk_read;
	blk->write	= romdisk_write;
	blk->sync	= romdisk_sync;
	blk->suspend = romdisk_suspend;
	blk->resume = romdisk_resume;
	blk->priv	= romdisk;

	if(!register_block(blk))
	{
		free(romdisk->name);
		free(romdisk);
		free(blk);
		return FALSE;
	}

	return TRUE;
}

static bool_t unregister_romdisk(const char * name)
{
	struct block_t * blk;
	struct romdisk_t * romdisk;

	if(!name)
		return FALSE;

	blk = search_block(name);
	if(!blk)
		return FALSE;

	romdisk = (struct romdisk_t *)(blk->priv);
	if(!unregister_block(blk))
		return FALSE;

	free(romdisk->name);
	free(romdisk);
	free(blk);
	return TRUE;
}

static __init void romdisk_init(void)
{
	register_romdisk("romdisk", __romdisk_start, __romdisk_end);
}

static __exit void romdisk_exit(void)
{
	unregister_romdisk("romdisk");
}

device_initcall(romdisk_init);
device_exitcall(romdisk_exit);
