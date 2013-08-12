/*
 * drivers/disk/disk.c
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
#include <stdio.h>
#include <xboot/printk.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>
#include <xboot/ioctl.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <disk/disk.h>

static struct disk_list_t __disk_list = {
	.entry = {
		.next	= &(__disk_list.entry),
		.prev	= &(__disk_list.entry),
	},
};
static struct disk_list_t * disk_list = &__disk_list;

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

static int disk_block_open(struct blkdev_t * dev)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->driver);

	if(dblk->busy == TRUE)
		return -1;

	dblk->busy = TRUE;

	return 0;
}

static ssize_t disk_block_read(struct blkdev_t * dev, u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->driver);
	struct disk_t * disk = dblk->disk;
	size_t offset = dblk->offset;

	return (disk->read_sectors(dblk->disk, buf, blkno + offset, blkcnt));
}

static ssize_t disk_block_write(struct blkdev_t * dev, const u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->driver);
	struct disk_t * disk = dblk->disk;
	size_t offset = dblk->offset;

	return (disk->write_sectors(dblk->disk, buf, blkno + offset, blkcnt));
}

static int disk_block_ioctl(struct blkdev_t * dev, int cmd, void * arg)
{
	return -1;
}

static int disk_block_close(struct blkdev_t * dev)
{
	struct disk_block_t * dblk = (struct disk_block_t *)(dev->driver);

	dblk->busy = FALSE;
	return 0;
}

static struct disk_t * search_disk(const char * name)
{
	struct disk_list_t * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list_t, entry);
		if(strcmp(list->disk->name, name) == 0)
			return list->disk;
	}

	return NULL;
}

bool_t register_disk(struct disk_t * disk, enum blkdev_type_t type)
{
	struct disk_list_t * list;
	struct partition_t * part;
	struct blkdev_t * dev;
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

	/*
	 * add disk to disk_list
	 */
	list->disk = disk;
	list_add(&list->entry, &disk_list->entry);

	/*
	 * register block device using partition information
	 */
	for(i = 0, part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); i++, part_pos = part_pos->next)
	{
		part = list_entry(part_pos, struct partition_t, entry);

		dev = malloc(sizeof(struct blkdev_t));
		dblk = malloc(sizeof(struct disk_block_t));
		if(!dev || !dblk)
		{
			/*
			 * please do not free 'list'.
			 */
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
		dev->type	= type;
		dev->blksz	= part->sector_size;
		dev->blkcnt	= part->sector_to - part->sector_from + 1;
		dev->open 	= disk_block_open;
		dev->read 	= disk_block_read;
		dev->write	= disk_block_write;
		dev->ioctl 	= disk_block_ioctl;
		dev->close	= disk_block_close;
		dev->driver = dblk;

		if(!register_blkdev(dev))
		{
			/*
			 * please do not free 'list'.
			 */
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
	struct disk_list_t * list;
	struct list_head * pos;
	struct partition_t * part;
	struct list_head * part_pos;
	struct blkdev_t * dev;
	struct disk_block_t * dblk;

	if(!disk || !disk->name)
		return FALSE;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list_t, entry);
		if(list->disk == disk)
		{
			for(part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); part_pos = part_pos->next)
			{
				part = list_entry(part_pos, struct partition_t, entry);
				dev = part->dev;
				dblk = dev->driver;

				unregister_blkdev(dblk->name);
				free(dblk);
				free(dev);
				free(part);
			}

			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * disk read function, just used by partition parser.
 */
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

static s32_t disk_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct disk_list_t * list;
	struct list_head * pos;
	struct partition_t * part;
	struct list_head * part_pos;
	char buff[32];
	u64_t from, to , size;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list_t, entry);
		len += sprintf((char *)(p + len), (const char *)"%s:\r\n", list->disk->name);

		for(part_pos = (&(list->disk->info.entry))->next; part_pos != &(list->disk->info.entry); part_pos = part_pos->next)
		{
			part = list_entry(part_pos, struct partition_t, entry);
			from = part->sector_from * part->sector_size;
			to = (part->sector_to + 1) * part->sector_size;
			size = to - from;
			len += sprintf((char *)(p + len), (const char *)" %8s %8s", part->name, part->dev->name);
			len += sprintf((char *)(p + len), (const char *)" 0x%016Lx ~ 0x%016Lx", from, to);
			ssize(buff, size);
			len += sprintf((char *)(p + len), (const char *)" %s\r\n", buff);
		}
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
	free(p);

	return len;
}

static struct proc_t disk_proc = {
	.name	= "disk",
	.read	= disk_proc_read,
};

static __init void disk_pure_sync_init(void)
{
	proc_register(&disk_proc);
}

static __exit void disk_pure_sync_exit(void)
{
	proc_unregister(&disk_proc);
}

pure_initcall_sync(disk_pure_sync_init);
pure_exitcall_sync(disk_pure_sync_exit);
