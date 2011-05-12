/*
 * drivers/loop/loop.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <div64.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/device.h>
#include <xboot/blkdev.h>
#include <xboot/ioctl.h>
#include <fs/fsapi.h>
#include <loop/loop.h>

/*
 * the struct of loop
 */
struct loop
{
	/* loop name */
	char name[32 + 1];

	/* file name's full path */
	char path[MAX_PATH];

	/* file descriptor */
	x_s32 fd;

	/* busy or not */
	x_bool busy;

	/* read only flag */
	x_bool read_only;
};

/*
 * the list of loop
 */
struct loop_list
{
	struct loop * loop;
	struct list_head entry;
};

/*
 * the list of loop
 */
static struct loop_list __loop_list = {
	.entry = {
		.next	= &(__loop_list.entry),
		.prev	= &(__loop_list.entry),
	},
};
static struct loop_list * loop_list = &__loop_list;


static x_s32 loop_open(struct blkdev * dev)
{
	struct loop * loop = (struct loop *)(dev->driver);

	if(loop->busy == TRUE)
		return -1;

	if(access((const char *)loop->path, R_OK) != 0)
		return -1;

	if(access((const char *)loop->path, W_OK) == 0)
	{
		loop->read_only = FALSE;
		loop->fd = open((const char *)loop->path, O_RDWR, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
		if(loop->fd < 0)
			return -1;
	}
	else
	{
		loop->read_only = TRUE;
		loop->fd = open((const char *)loop->path, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
		if(loop->fd < 0)
			return -1;
	}

	loop->busy = TRUE;
	return 0;
}

static x_s32 loop_read(struct blkdev * dev, x_u8 * buf, x_u32 blkno, x_u32 blkcnt)
{
	struct loop * loop = (struct loop *)(dev->driver);
	x_off offset = get_blkdev_offset(dev, blkno);
	x_size size = get_blkdev_size(dev) * blkcnt;

	if(offset < 0)
		return 0;

	if(size < 0)
		return 0;

	if(lseek(loop->fd, offset, SEEK_SET) < 0)
		return 0;

	if(read(loop->fd, (void *)buf, size) != size)
		return 0;

	return blkcnt;
}

static x_s32 loop_write(struct blkdev * dev, const x_u8 * buf, x_u32 blkno, x_u32 blkcnt)
{
	struct loop * loop = (struct loop *)(dev->driver);
	x_off offset = get_blkdev_offset(dev, blkno);
	x_size size = get_blkdev_size(dev) * blkcnt;

	if(loop->read_only == TRUE)
		return 0;

	if(lseek(loop->fd, offset, SEEK_SET) < 0)
		return 0;

	if(write(loop->fd, (void *)buf, size) != size)
		return 0;

	return blkcnt;
}

static x_s32 loop_ioctl(struct blkdev * dev, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 loop_close(struct blkdev * dev)
{
	struct loop * loop = (struct loop *)(dev->driver);

	if(close(loop->fd) == 0)
	{
		loop->busy = FALSE;
		return 0;
	}

	return -1;
}

/*
 * search loop block device by file name
 */
struct blkdev * search_loop(const char * file)
{
	struct loop_list * list;
	struct list_head * pos;
	char buf[MAX_PATH];

	if(!file)
		return NULL;

	if(vfs_path_conv(file, buf) !=0)
		return NULL;

	for(pos = (&loop_list->entry)->next; pos != (&loop_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct loop_list, entry);
		if(strcmp((char*)list->loop->path, (const char *)buf) == 0)
		{
			return search_blkdev_with_type(list->loop->name, BLK_DEV_LOOP);
		}
	}

	return NULL;
}

/*
 * register a file as a loop block device, return true on success.
 */
x_bool register_loop(const char * file)
{
	struct stat st;
	struct blkdev * dev;
	struct loop * loop;
	struct loop_list * list;
	x_u64 size, rem;
	x_s32 i = 0;

	if(!file)
		return FALSE;

	if(stat(file, &st) != 0)
		return FALSE;

	if(st.st_size <= 0)
		return FALSE;

	if(!S_ISREG(st.st_mode))
		return FALSE;

	list = malloc(sizeof(struct loop_list));
	if(!list)
		return FALSE;

	dev = malloc(sizeof(struct blkdev));
	if(!dev)
	{
		free(list);
		return FALSE;
	}

	loop = malloc(sizeof(struct loop));
	if(!loop)
	{
		free(dev);
		free(list);
		return FALSE;
	}

	while(1)
	{
		snprintf((char *)loop->name, 32, (const char *)"loop%ld", i++);
		if(search_device(loop->name) == NULL)
			break;
	}

	if(vfs_path_conv(file, loop->path) !=0)
	{
		free(loop);
		free(dev);
		free(list);
		return FALSE;
	}

	size = st.st_size;
	rem = div64_64(&size, SZ_512);
	if(rem > 0)
		size++;

	loop->busy 		= FALSE;
	loop->read_only	= FALSE;

	dev->name		= loop->name;
	dev->type		= BLK_DEV_LOOP;
	dev->blksz		= SZ_512;
	dev->blkcnt		= size;
	dev->open 		= loop_open;
	dev->read		= loop_read;
	dev->write		= loop_write;
	dev->ioctl 		= loop_ioctl;
	dev->close		= loop_close;
	dev->driver 	= loop;

	list->loop 	= loop;

	if(!register_blkdev(dev))
	{
		free(loop);
		free(dev);
		free(list);
		return FALSE;
	}

	list_add(&list->entry, &loop_list->entry);

	return TRUE;
}

/*
 * unregister loop block device
 */
x_bool unregister_loop(const char * file)
{
	struct loop_list * list;
	struct list_head * pos;
	struct blkdev * dev;
	char buf[MAX_PATH];

	if(!file)
		return FALSE;

	if(vfs_path_conv(file, buf) !=0)
		return FALSE;

	for(pos = (&loop_list->entry)->next; pos != (&loop_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct loop_list, entry);
		if(strcmp((char*)list->loop->path, (const char *)buf) == 0)
		{
			dev = search_blkdev_with_type(list->loop->name, BLK_DEV_LOOP);
			if(dev)
			{
				if(unregister_blkdev(list->loop->name))
				{
					free(list->loop);
					free(dev);
					list_del(pos);
					free(list);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/*
 * loop proc interface
 */
static x_s32 loop_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct loop_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[loop]");

	for(pos = (&loop_list->entry)->next; pos != (&loop_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct loop_list, entry);
		len += sprintf((char *)(p + len), (const char *)"\r\n %s %s ", list->loop->name, list->loop->path);
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

static struct proc loop_proc = {
	.name	= "loop",
	.read	= loop_proc_read,
};

/*
 * loop pure sync init
 */
static __init void loop_pure_sync_init(void)
{
	/* register loop proc interface */
	proc_register(&loop_proc);
}

static __exit void loop_pure_sync_exit(void)
{
	/* unregister loop proc interface */
	proc_unregister(&loop_proc);
}

module_init(loop_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(loop_pure_sync_exit, LEVEL_PURE_SYNC);
