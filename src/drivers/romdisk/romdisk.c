/*
 * drivers/romdisk/romdisk.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>
#include <xboot/ioctl.h>

extern u8_t __romdisk_start[];
extern u8_t __romdisk_end[];

/*
 * the struct of romdisk_t
 */
struct romdisk_t
{
	/* the romdisk name */
	char name[32 + 1];

	/* the start of romdisk */
	void * start;

	/* the end of romdisk */
	void * end;

	/* busy or not */
	bool_t busy;
};

static int romdisk_open(struct blkdev_t * dev)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(dev->driver);

	if(romdisk->busy == TRUE)
		return -1;

	romdisk->busy = TRUE;
	return 0;
}

static ssize_t romdisk_read(struct blkdev_t * dev, u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(dev->driver);
	u8_t * p = (u8_t *)(romdisk->start);
	loff_t offset = get_blkdev_offset(dev, blkno);
	size_t size = get_blkdev_size(dev) * blkcnt;

	if(offset < 0)
		return 0;

	if(size < 0)
		return 0;

	memcpy((void *)buf, (const void *)(p + offset), size);
	return blkcnt;
}

static ssize_t romdisk_write(struct blkdev_t * dev, const u8_t * buf, size_t blkno, size_t blkcnt)
{
	return 0;
}

static int romdisk_ioctl(struct blkdev_t * dev, int cmd, void * arg)
{
	return -1;
}

static int romdisk_close(struct blkdev_t * dev)
{
	struct romdisk_t * romdisk = (struct romdisk_t *)(dev->driver);

	romdisk->busy = FALSE;
	return 0;
}

static __init void romdisk_init(void)
{
	struct blkdev_t * dev;
	struct romdisk_t * romdisk;
	u64_t size, rem;

	dev = malloc(sizeof(struct blkdev_t));
	if(!dev)
		return;

	romdisk = malloc(sizeof(struct romdisk_t));
	if(!romdisk)
	{
		free(dev);
		return;
	}

	snprintf(romdisk->name, 32, "romdisk");
	romdisk->start = (void *)__romdisk_start;
	romdisk->end = (void *)__romdisk_end;
	romdisk->busy = FALSE;

	if((romdisk->end - romdisk->start) <= 0)
	{
		free(romdisk);
		free(dev);
		return;
	}

	size = (u64_t)(romdisk->end - romdisk->start);
	rem = size % SZ_512;
	size = size / SZ_512;
	if(rem > 0)
		size++;

	romdisk->busy	= FALSE;

	dev->name		= romdisk->name;
	dev->type		= BLK_DEV_ROMDISK;
	dev->blksz		= SZ_512;
	dev->blkcnt		= size;
	dev->open 		= romdisk_open;
	dev->read 		= romdisk_read;
	dev->write		= romdisk_write;
	dev->ioctl 		= romdisk_ioctl;
	dev->close		= romdisk_close;
	dev->driver	 = romdisk;

	if(!register_blkdev(dev))
	{
		free(romdisk);
		free(dev);
		return;
	}
}

static __exit void romdisk_exit(void)
{
	struct blkdev_t * dev;
	struct romdisk_t * romdisk;

	dev = search_blkdev_with_type("romdisk", BLK_DEV_ROMDISK);
	if(dev)
	{
		romdisk = (struct romdisk_t *)(dev->driver);

		if(unregister_blkdev(dev->name))
		{
			free(romdisk);
			free(dev);
			return;
		}
	}
}

device_initcall(romdisk_init);
device_exitcall(romdisk_exit);
