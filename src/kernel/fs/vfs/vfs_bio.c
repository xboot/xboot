/*
 * kernel/fs/vfs/vfs_bio.c
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
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <xboot/initcall.h>
#include <xboot/proc.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

/*
 * bio flags
 */
enum bio_flag {
	BIO_FLAG_READ,
	BIO_FLAG_WRITE,
};

/*
 * the struct bio, for io cache.
 */
struct bio
{
	/* block device pointer */
	struct blkdev * dev;

	/* block's no */
	x_s32 blkno;

	/* block's offset */
	x_off offset;

	/* block's size */
	x_s32 size;

	/* pointer to buffer */
	x_u8 * buf;

	/* mark flags */
	enum bio_flag flag;
};

/*
 * the list of bio
 */
struct bio_list
{
	struct bio * bio;
	struct list_head entry;
};

/* the list of bio */
static struct bio_list __bio_list = {
	.entry = {
		.next	= &(__bio_list.entry),
		.prev	= &(__bio_list.entry),
	},
};
static struct bio_list * bio_list = &__bio_list;
static x_s32 bio_numberof = 0;


static struct bio * search_bio(struct blkdev * dev, x_s32 blkno)
{
	struct bio_list * list;
	struct list_head * pos;

	if(!dev || (blkno < 0))
		return NULL;

	for(pos = (&bio_list->entry)->next; pos != (&bio_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct bio_list, entry);
		if(list->bio->dev == dev && list->bio->blkno == blkno)
			return list->bio;
	}

	return NULL;
}

static struct bio * add_bio(struct blkdev * dev, x_s32 blkno)
{
	struct bio * bio;
	struct bio_list * list;
	struct bio_list * remove_list;
	struct list_head * remove_pos;
	x_s32 size;

	if(!dev || !dev->info || (blkno < 0))
		return NULL;

	if((bio = search_bio(dev, blkno)) != NULL)
		return bio;

	size = get_blkdev_size(dev, blkno);
	if(size <= 0)
		return NULL;

	list = malloc(sizeof(struct bio_list));
	if(!list)
		return NULL;

	bio = malloc(sizeof(struct bio));
	if(!bio)
	{
		free(list);
		return NULL;
	}

	bio->buf = malloc(size);
	if(!bio->buf)
	{
		free(bio);
		free(list);
		return NULL;
	}

	bio->dev = dev;
	bio->blkno = blkno;
	bio->offset = get_blkdev_offset(dev, blkno);
	bio->size = size;
	bio->flag = BIO_FLAG_READ;
	memset(bio->buf, 0, size);

	if( (!dev->read) || (dev->read(dev, bio->buf, blkno) <= 0) )
	{
		free(bio->buf);
		free(bio);
		free(list);
		return NULL;
	}

	if(bio_numberof >= CONFIG_MAX_NUMBER_OF_VFS_BIO)
	{
		if(!list_empty(&bio_list->entry))
		{
			remove_pos = (&bio_list->entry)->next;
			remove_list = list_entry(remove_pos, struct bio_list, entry);

			if(remove_list->bio->flag == BIO_FLAG_WRITE)
			{
				if(dev->write)
					dev->write(dev, list->bio->buf, blkno);
			}

			list_del(remove_pos);
			free(remove_list->bio->buf);
			free(remove_list->bio);
			free(remove_list);
			bio_numberof--;
		}
	}

	list->bio = bio;
	list_add_tail(&list->entry, &bio_list->entry);

	bio_numberof++;
	return bio;
}

/*
 * sync bio
 */
void bio_sync(void)
{
	struct bio_list * list;
	struct list_head * head, * curr, * next;

	head = &bio_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct bio_list, entry);

		next = curr->next;
		if(list->bio->flag == BIO_FLAG_WRITE)
		{
			if(list->bio->dev->write)
				list->bio->dev->write(list->bio->dev, list->bio->buf, list->bio->blkno);

			list_del(curr);
			free(list->bio->buf);
			free(list->bio);
			free(list);
			bio_numberof--;
		}
		curr = next;
	}
}

/*
 * flush bio
 */
void bio_flush(struct blkdev * dev)
{
	struct bio_list * list;
	struct list_head * head, * curr, * next;

	if(!dev)
		return;

	head = &bio_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct bio_list, entry);

		next = curr->next;
		if(list->bio->dev == dev)
		{
			if(list->bio->flag == BIO_FLAG_WRITE)
			{
				if(list->bio->dev->write)
					list->bio->dev->write(list->bio->dev, list->bio->buf, list->bio->blkno);
			}

			list_del(curr);
			free(list->bio->buf);
			free(list->bio);
			free(list);
			bio_numberof--;
		}
		curr = next;
	}
}

/*
 * read bio
 */
x_size bio_read(struct blkdev * dev, x_u8 * buf, x_off offset, x_size count)
{
	struct bio * bio;
	x_s32 blkno;
	x_u8 * p = buf;
	x_s32 o = 0, l = 0;
	x_size len = 0;

	if(!dev || !dev->info || !buf || (offset < 0) || (count <= 0) )
		return 0;

	while(len < count)
	{
		blkno = get_blkdev_blkno(dev, offset);
		bio = add_bio(dev, blkno);
		if(bio == NULL)
			return len;

		o = offset - bio->offset;
		l = bio->size - o;
		if(len + l > count)
			l = count - len;

		memcpy((void *)p, (const void *)(&bio->buf[o]), l);

		offset += l;
		p += l;
		len += l;
	}

	return len;
}

/*
 * write bio
 */
x_size bio_write(struct blkdev * dev, const x_u8 * buf, x_off offset, x_size count)
{
	struct bio * bio;
	x_s32 blkno;
	x_u8 * p = (x_u8 *)buf;
	x_s32 o = 0, l = 0;
	x_size len = 0;

	if(!dev || !dev->info || !buf || (offset < 0) || (count <= 0) )
		return 0;

	while(len < count)
	{
		blkno = get_blkdev_blkno(dev, offset);
		bio = add_bio(dev, blkno);
		if(bio == NULL)
			return len;

		o = offset - bio->offset;
		l = bio->size - o;
		if(len + l > count)
			l = count - len;

		memcpy((void *)(&bio->buf[o]), (const void *)p, l);
		bio->flag = BIO_FLAG_WRITE;

		offset += l;
		p += l;
		len += l;
	}

	return 0;
}

/*
 * bio proc interface
 */
static x_s32 bio_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	x_s8 tmp[16];
	x_s32 len;

	len = sprintf(tmp, (const x_s8 *)"%ld", bio_numberof);
	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, &tmp[offset], len);

	return len;
}

static struct proc bio_proc = {
	.name	= "bio",
	.read	= bio_proc_read,
};

/*
 * bio pure sync init
 */
static __init void bio_pure_sync_init(void)
{
	/* register bio proc interface */
	proc_register(&bio_proc);
}

static __exit void bio_pure_sync_exit(void)
{
	/* unregister bio proc interface */
	proc_unregister(&bio_proc);
}

module_init(bio_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(bio_pure_sync_exit, LEVEL_PURE_SYNC);
