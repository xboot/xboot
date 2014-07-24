/*
 * drivers/block/romdisk/romdisk.c
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
#include <block/block.h>

extern u8_t __romdisk_start[];
extern u8_t __romdisk_end[];

struct romdisk_t
{
	/* The romdisk name */
	char * name;

	/* The start of romdisk */
	void * start;

	/* The end of romdisk */
	void * end;

	/* Busy or not */
	bool_t busy;
};

static int romdisk_open(struct block_t * blk)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(blk->priv);

	if(romdisk->busy == TRUE)
		return -1;

	romdisk->busy = TRUE;
	return 0;
}

static ssize_t romdisk_read(struct block_t * blk, u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(blk->priv);
	u8_t * p = (u8_t *)(romdisk->start);
	loff_t offset = get_block_offset(blk, blkno);
	size_t size = get_block_size(blk) * blkcnt;

	if(offset < 0)
		return 0;

	if(size < 0)
		return 0;

	memcpy((void *)buf, (const void *)(p + offset), size);
	return blkcnt;
}

static ssize_t romdisk_write(struct block_t * blk, const u8_t * buf, size_t blkno, size_t blkcnt)
{
	return 0;
}

static int romdisk_close(struct block_t * blk)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(blk->priv);

	romdisk->busy = FALSE;
	return 0;
}

static bool_t register_romdisk(const char * name, void * start, void * end)
{
	struct block_t * blk;
	struct romdisk_t * romdisk;
	size_t size;

	if(!name)
		return FALSE;

	size = (size_t)(end - start);
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

	romdisk->name = (char *)name;
	romdisk->start = start;
	romdisk->end = end;
	romdisk->busy = FALSE;

	blk->name	= romdisk->name;
	blk->blksz	= SZ_512;
	blk->blkcnt	= size;
	blk->open 	= romdisk_open;
	blk->read 	= romdisk_read;
	blk->write	= romdisk_write;
	blk->close	= romdisk_close;
	blk->priv	= romdisk;

	if(!register_block(blk))
	{
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
