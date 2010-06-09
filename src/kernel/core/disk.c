/*
 * kernel/core/disk.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <xboot.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/printk.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>
#include <xboot/ioctl.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/disk.h>

/* the list of disk */
static struct disk_list __disk_list = {
	.entry = {
		.next	= &(__disk_list.entry),
		.prev	= &(__disk_list.entry),
	},
};
static struct disk_list * disk_list = &__disk_list;

/*
 * the struct of disk_block
 */
struct disk_block
{
	/* block device name */
	char name[32+1];

	/* block information */
	struct blkinfo info;

	/* partition information */
	struct partition * part;

	/* the offset of sector for this partition */
	x_u32 offset;

	/* busy or not */
	x_bool busy;

	/* point to the disk */
	struct disk * disk;
};

static x_s32 disk_block_open(struct blkdev * dev)
{
	struct disk_block * dblk = (struct disk_block *)(dev->driver);

	if(dblk->busy == TRUE)
		return -1;

	dblk->busy = TRUE;

	return 0;
}

static x_s32 disk_block_read(struct blkdev * dev, x_u8 * buf, x_s32 blkno)
{
	struct disk_block * dblk = (struct disk_block *)(dev->driver);
	struct disk * disk = dblk->disk;
	x_u32 offset = dblk->offset;

	if(blkno < 0)
		return 0;

	if(disk->read_sector(dblk->disk, blkno + offset, buf) == FALSE)
		return 0;

	return disk->sector_size;
}

static x_s32 disk_block_write(struct blkdev * dev, const x_u8 * buf, x_s32 blkno)
{
	struct disk_block * dblk = (struct disk_block *)(dev->driver);
	struct disk * disk = dblk->disk;
	x_u32 offset = dblk->offset;

	if(blkno < 0)
		return 0;

	if(disk->write_sector(dblk->disk, blkno + offset, (x_u8 *)buf) == FALSE)
		return 0;

	return disk->sector_size;
}

static x_s32 disk_block_ioctl(struct blkdev * dev, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 disk_block_close(struct blkdev * dev)
{
	struct disk_block * dblk = (struct disk_block *)(dev->driver);

	dblk->busy = FALSE;
	return 0;
}

/*
 * search disk by name
 */
static struct disk * search_disk(const char * name)
{
	struct disk_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list, entry);
		if(strcmp((x_s8*)list->disk->name, (const x_s8 *)name) == 0)
			return list->disk;
	}

	return NULL;
}

/*
 * register a disk into disk_list
 */
x_bool register_disk(struct disk * disk, enum blkdev_type type)
{
	struct disk_list * list;
	struct partition * part;
	struct blkdev * dev;
	struct disk_block * dblk;
	struct blkinfo * info;
	struct list_head * part_pos;
	x_s32 i;

	if(!disk || !disk->name || search_disk(disk->name))
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sector) || (!disk->write_sector))
		return FALSE;

	if(!partition_parser_probe(disk))
		return FALSE;

	if(list_empty(&(disk->info.entry)))
		return FALSE;

	list = malloc(sizeof(struct disk_list));
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
		part = list_entry(part_pos, struct partition, entry);

		dev = malloc(sizeof(struct blkdev));
		dblk = malloc(sizeof(struct disk_block));
		info = malloc(sizeof(struct blkinfo));
		if(!dev || !dblk || !info)
		{
			/*
			 * please do not free 'list'.
			 */
			free(dev);
			free(dblk);
			free(info);
			unregister_disk(disk);

			return FALSE;
		}

		if(i == 0)
			snprintf((x_s8 *)dblk->name, sizeof(dblk->name), (const x_s8 *)"%s", disk->name);
		else
			snprintf((x_s8 *)dblk->name, sizeof(dblk->name), (const x_s8 *)"%sp%ld", disk->name, i);

		init_list_head(&(dblk->info.entry));
		info->blkno = 0;
		info->offset = 0;
		info->size = part->sector_size;
		info->number = part->sector_to - part->sector_from + 1;
		list_add_tail(&info->entry, &(dblk->info.entry));
		part->dev = dev;
		dblk->part = part;
		dblk->offset = part->sector_from;
		dblk->busy = FALSE;
		dblk->disk = disk;

		dev->name	= dblk->name;
		dev->type	= type;
		dev->info	= &(dblk->info);
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
			free(info);
			unregister_disk(disk);

			return FALSE;
		}
	}

	return TRUE;
}

/*
 * unregister disk from disk_list
 */
x_bool unregister_disk(struct disk * disk)
{
	struct disk_list * list;
	struct list_head * pos;
	struct partition * part;
	struct list_head * part_pos;
	struct blkinfo * info;
	struct list_head * info_pos;
	struct blkdev * dev;
	struct disk_block * dblk;

	if(!disk || !disk->name)
		return FALSE;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list, entry);
		if(list->disk == disk)
		{
			for(part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); part_pos = part_pos->next)
			{
				part = list_entry(part_pos, struct partition, entry);
				dev = part->dev;
				dblk = dev->driver;
				if(unregister_blkdev(dblk->name))
				{
					for(info_pos = (&(dblk->info.entry))->next; info_pos != &(dblk->info.entry); info_pos = info_pos->next)
					{
						info = list_entry(info_pos, struct blkinfo, entry);
						free(info);
					}
				}
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
x_bool disk_read(struct disk * disk, x_u8 * buf, x_u32 offset, x_u32 size)
{
	x_u8 * sector_buf;
	x_u8 * p = buf;
	x_u32 sector_size;
	x_u32 sector, len = 0;
	x_u32 o = 0, l = 0;

	if(!disk)
		return FALSE;

	if( (buf == NULL) || (size <= 0) )
		return FALSE;

	sector_size = disk->sector_size;
	if(sector_size <= 0)
		return FALSE;

	sector_buf = malloc(disk->sector_size);
	if(!sector_buf)
		return FALSE;

	while(len < size)
	{
		sector = offset / sector_size;
		o = offset % sector_size;
		l = sector_size - o;

		if(len + l > size)
			l = size - len;

		if(disk->read_sector(disk, sector, sector_buf) == FALSE)
		{
			free(sector_buf);
			return FALSE;
		}

		memcpy((void *)p, (const void *)(&sector_buf[o]), l);

		offset += l;
		p += l;
		len += l;
	}

	free(sector_buf);
	return TRUE;
}

/*
 * disk proc interface
 */
static x_s32 disk_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct disk_list * list;
	struct list_head * pos;
	struct partition * part;
	struct list_head * part_pos;
	x_s8 buff[32];
	x_u64 from, to , size;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&disk_list->entry)->next; pos != (&disk_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct disk_list, entry);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"%s:\r\n", list->disk->name);

		for(part_pos = (&(list->disk->info.entry))->next; part_pos != &(list->disk->info.entry); part_pos = part_pos->next)
		{
			part = list_entry(part_pos, struct partition, entry);
			from = part->sector_from * part->sector_size;
			to = (part->sector_to + 1) * part->sector_size;
			size = to - from;
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)" %8s %8s", part->name, part->dev->name);
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)" 0x%016Lx ~ 0x%016Lx", from, to);
			ssize(buff, size);
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)" %s\r\n", buff);
		}
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc disk_proc = {
	.name	= "disk",
	.read	= disk_proc_read,
};

/*
 * disk pure sync init
 */
static __init void disk_pure_sync_init(void)
{
	/* register disk proc interface */
	proc_register(&disk_proc);
}

static __exit void disk_pure_sync_exit(void)
{
	/* unregister disk proc interface */
	proc_unregister(&disk_proc);
}

module_init(disk_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(disk_pure_sync_exit, LEVEL_PURE_SYNC);
