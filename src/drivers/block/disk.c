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

#include <xboot.h>
#include <spinlock.h>
#include <block/disk.h>

static struct disk_list_t __disk_list = {
	.entry = {
		.next	= &(__disk_list.entry),
		.prev	= &(__disk_list.entry),
	},
};
static spinlock_t __disk_list_lock = SPIN_LOCK_INIT();

struct disk_block_t
{
	/* block device name */
	char name[32 + 1];

	/* partition information */
	struct partition_t * part;

	/* the offset of sector for this partition */
	size_t offset;

	/* busy or not */
	bool_t busy;

	/* point to the disk */
	struct disk_t * disk;
};

static int disk_block_open(struct block_t * dev)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->priv);

	if(dblk->busy == TRUE)
		return -1;

	dblk->busy = TRUE;

	return 0;
}

static ssize_t disk_block_read(struct block_t * dev, u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->priv);
	struct disk_t * disk = dblk->disk;
	size_t offset = dblk->offset;

	return (disk->read_sectors(dblk->disk, buf, blkno + offset, blkcnt));
}

static ssize_t disk_block_write(struct block_t * dev, const u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->priv);
	struct disk_t * disk = dblk->disk;
	size_t offset = dblk->offset;

	return (disk->write_sectors(dblk->disk, buf, blkno + offset, blkcnt));
}

static int disk_block_close(struct block_t * dev)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->priv);

	dblk->busy = FALSE;
	return 0;
}

static struct disk_t * search_disk(const char * name)
{
	struct disk_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__disk_list.entry), entry)
	{
		if(strcmp(pos->disk->name, name) == 0)
			return pos->disk;
	}

	return NULL;
}

bool_t register_disk(struct disk_t * disk)
{
	struct disk_list_t * list;
	struct partition_t * part;
	struct block_t * dev;
	struct disk_block_t * dblk;
	struct list_head * part_pos;
	s32_t i;

	if(!disk || !disk->name || search_disk(disk->name))
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sectors) || (!disk->write_sectors))
		return FALSE;

	if(!partition_parser_probe(disk))
		return FALSE;

	if(list_empty(&(disk->info.entry)))
		return FALSE;

	list = malloc(sizeof(struct disk_list_t));
	if(!list)
		return FALSE;

	list->disk = disk;

	spin_lock_irq(&__disk_list_lock);
	list_add_tail(&list->entry, &(__disk_list.entry));
	spin_unlock_irq(&__disk_list_lock);

	for(i = 0, part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); i++, part_pos = part_pos->next)
	{
		part = list_entry(part_pos, struct partition_t, entry);

		dev = malloc(sizeof(struct block_t));
		dblk = malloc(sizeof(struct disk_block_t));
		if(!dev || !dblk)
		{
			free(dev);
			free(dblk);
			unregister_disk(disk);

			return FALSE;
		}

		if(i == 0)
			snprintf((char *)dblk->name, sizeof(dblk->name), (const char *)"%s", disk->name);
		else
			snprintf((char *)dblk->name, sizeof(dblk->name), (const char *)"%sp%d", disk->name, i);

		part->dev = dev;
		dblk->part = part;
		dblk->offset = part->sector_from;
		dblk->busy = FALSE;
		dblk->disk = disk;

		dev->name	= dblk->name;
		dev->blksz	= part->sector_size;
		dev->blkcnt	= part->sector_to - part->sector_from + 1;
		dev->open 	= disk_block_open;
		dev->read 	= disk_block_read;
		dev->write	= disk_block_write;
		dev->close	= disk_block_close;
		dev->priv	= dblk;

		if(!register_block(dev))
		{
			free(dev);
			free(dblk);
			unregister_disk(disk);

			return FALSE;
		}
	}

	return TRUE;
}

bool_t unregister_disk(struct disk_t * disk)
{
	struct disk_list_t * pos, * n;
	struct partition_t * part;
	struct list_head * part_pos;
	struct block_t * dev;
	struct disk_block_t * dblk;

	if(!disk || !disk->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__disk_list.entry), entry)
	{
		if(pos->disk == disk)
		{
			for(part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); part_pos = part_pos->next)
			{
				part = list_entry(part_pos, struct partition_t, entry);
				dev = part->dev;
				dblk = dev->priv;

				unregister_block(dev);
				free(dblk);
				free(dev);
				free(part);
			}

			spin_lock_irq(&__disk_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__disk_list_lock);
			free(pos);

			return TRUE;
		}
	}

	return FALSE;
}

loff_t disk_read(struct disk_t * disk, u8_t * buf, loff_t offset, loff_t count)
{
	u8_t * secbuf;
	size_t secno, secsz, seccnt;
	u64_t div, rem;
	size_t len;
	loff_t tmp;
	loff_t size = 0;

	if(!buf)
		return 0;

	if(!disk)
		return 0;

	secsz = disk->sector_size;
	if(secsz <= 0)
		return 0;

	seccnt = disk->sector_count;
	if(seccnt <= 0)
		return 0;

	tmp = secsz * seccnt;
	if( (count <= 0) || (offset < 0) || (offset >= tmp) )
		return 0;

	tmp = tmp - offset;
	if(count > tmp)
		count = tmp;

	secbuf = malloc(secsz);
	if(!secbuf)
		return 0;

	div = offset;
	rem = div % secsz;
	div = div / secsz;
	secno = div;

	if(rem > 0)
	{
		len = secsz - rem;
		if(count < len)
			len = count;

		if(disk->read_sectors(disk, secbuf, secno, 1) != 1)
		{
			free(secbuf);
			return 0;
		}

		memcpy((void *)buf, (const void *)(&secbuf[rem]), len);
		buf += len;
		count -= len;
		size += len;
		secno += 1;
	}

	div = count;
	rem = div % secsz;
	div = div / secsz;

	if(div > 0)
	{
		len = div * secsz;

		if(disk->read_sectors(disk, buf, secno, div) != div)
		{
			free(secbuf);
			return size;
		}

		buf += len;
		count -= len;
		size += len;
		secno += div;
	}

	if(count > 0)
	{
		len = count;

		if(disk->read_sectors(disk, secbuf, secno, 1) != 1)
		{
			free(secbuf);
			return size;
		}

		memcpy((void *)buf, (const void *)(&secbuf[0]), len);
		size += len;
	}

	free(secbuf);
	return size;
}
