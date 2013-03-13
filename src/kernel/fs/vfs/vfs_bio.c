/*
 * kernel/fs/vfs/vfs_bio.c
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
#include <malloc.h>
#include <div64.h>
#include <xboot/list.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <xboot/initcall.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

loff_t bio_read(struct blkdev * dev, u8_t * buf, loff_t offset, loff_t count)
{
	u8_t * blkbuf;
	size_t blkno, blksz, blkcnt;
	u64_t div, rem;
	size_t len;
	loff_t tmp;
	loff_t size = 0;

	if(!buf)
		return 0;

	if(!dev)
		return 0;

	blksz = get_blkdev_size(dev);
	if(blksz <= 0)
		return 0;

	blkcnt = get_blkdev_total_count(dev);
	if(blkcnt <= 0)
		return 0;

	tmp = get_blkdev_total_size(dev);
	if( (count <= 0) || (offset < 0) || (offset >= tmp) )
		return 0;

	tmp = tmp - offset;
	if(count > tmp)
		count = tmp;

	blkbuf = malloc(blksz);
	if(!blkbuf)
		return 0;

	div = offset;
	rem = div64_64(&div, blksz);
	blkno = div;

	if(rem > 0)
	{
		len = blksz - rem;
		if(count < len)
			len = count;

		if(dev->read(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return 0;
		}

		memcpy((void *)buf, (const void *)(&blkbuf[rem]), len);
		buf += len;
		count -= len;
		size += len;
		blkno += 1;
	}

	div = count;
	rem = div64_64(&div, blksz);

	if(div > 0)
	{
		len = div * blksz;

		if(dev->read(dev, buf, blkno, div) != div)
		{
			free(blkbuf);
			return size;
		}

		buf += len;
		count -= len;
		size += len;
		blkno += div;
	}

	if(count > 0)
	{
		len = count;

		if(dev->read(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return size;
		}

		memcpy((void *)buf, (const void *)(&blkbuf[0]), len);
		size += len;
	}

	free(blkbuf);
	return size;
}

loff_t bio_write(struct blkdev * dev, u8_t * buf, loff_t offset, loff_t count)
{
	u8_t * blkbuf;
	size_t blkno, blksz, blkcnt;
	u64_t div, rem;
	size_t len;
	loff_t tmp;
	loff_t size = 0;

	if(!buf)
		return 0;

	if(!dev)
		return 0;

	blksz = get_blkdev_size(dev);
	if(blksz <= 0)
		return 0;

	blkcnt = get_blkdev_total_count(dev);
	if(blkcnt <= 0)
		return 0;

	tmp = get_blkdev_total_size(dev);
	if( (count <= 0) || (offset < 0) || (offset >= tmp) )
		return 0;

	tmp = tmp - offset;
	if(count > tmp)
		count = tmp;

	blkbuf = malloc(blksz);
	if(!blkbuf)
		return 0;

	div = offset;
	rem = div64_64(&div, blksz);
	blkno = div;

	if(rem > 0)
	{
		len = blksz - rem;
		if(count < len)
			len = count;

		if(dev->read(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return 0;
		}

		memcpy((void *)(&blkbuf[rem]), (const void *)buf, len);

		if(dev->write(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return 0;
		}

		buf += len;
		count -= len;
		size += len;
		blkno += 1;
	}

	div = count;
	rem = div64_64(&div, blksz);

	if(div > 0)
	{
		len = div * blksz;

		if(dev->write(dev, buf, blkno, div) != div)
		{
			free(blkbuf);
			return size;
		}

		buf += len;
		count -= len;
		size += len;
		blkno += div;
	}

	if(count > 0)
	{
		len = count;

		if(dev->read(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return size;
		}

		memcpy((void *)(&blkbuf[0]), (const void *)buf, len);

		if(dev->write(dev, blkbuf, blkno, 1) != 1)
		{
			free(blkbuf);
			return size;
		}

		size += len;
	}

	free(blkbuf);
	return size;
}
