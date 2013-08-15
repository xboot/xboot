/*
 * drivers/loop/loop.c
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
#include <block/block.h>
#include <xboot/ioctl.h>
#include <fs/fileio.h>
#include <loop/loop.h>

struct loop_t
{
	/* loop name */
	char name[32 + 1];

	/* file name's full path */
	char path[MAX_PATH];

	/* file descriptor */
	s32_t fd;

	/* busy or not */
	bool_t busy;

	/* read only flag */
	bool_t read_only;
};

struct loop_list_t
{
	struct loop_t * loop;
	struct list_head entry;
};

static struct loop_list_t __loop_list = {
	.entry = {
		.next	= &(__loop_list.entry),
		.prev	= &(__loop_list.entry),
	},
};
static struct loop_list_t * loop_list = &__loop_list;

static int loop_open(struct block_t * dev)
{
	struct loop_t * loop = (struct loop_t *)(dev->priv);

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

static ssize_t loop_read(struct block_t * dev, u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct loop_t * loop = (struct loop_t *)(dev->priv);
	loff_t offset = get_block_offset(dev, blkno);
	loff_t size = get_block_size(dev) * blkcnt;

	if(offset < 0)
		return 0;

	if(size < 0)
		return 0;

	if(lseek(loop->fd, offset, VFS_SEEK_SET) < 0)
		return 0;

	if(read(loop->fd, (void *)buf, size) != size)
		return 0;

	return blkcnt;
}

static ssize_t loop_write(struct block_t * dev, const u8_t * buf, size_t blkno, size_t blkcnt)
{
	struct loop_t * loop = (struct loop_t *)(dev->priv);
	loff_t offset = get_block_offset(dev, blkno);
	loff_t size = get_block_size(dev) * blkcnt;

	if(loop->read_only == TRUE)
		return 0;

	if(lseek(loop->fd, offset, VFS_SEEK_SET) < 0)
		return 0;

	if(write(loop->fd, (void *)buf, size) != size)
		return 0;

	return blkcnt;
}

static int loop_close(struct block_t * dev)
{
	struct loop_t * loop = (struct loop_t *)(dev->priv);

	if(close(loop->fd) == 0)
	{
		loop->busy = FALSE;
		return 0;
	}

	return -1;
}

bool_t register_loop(const char * file)
{
	struct stat st;
	struct block_t * dev;
	struct loop_t * loop;
	struct loop_list_t * list;
	u64_t size, rem;
	s32_t i = 0;

	if(!file)
		return FALSE;

	if(stat(file, &st) != 0)
		return FALSE;

	if(st.st_size <= 0)
		return FALSE;

	if(!S_ISREG(st.st_mode))
		return FALSE;

	list = malloc(sizeof(struct loop_list_t));
	if(!list)
		return FALSE;

	dev = malloc(sizeof(struct block_t));
	if(!dev)
	{
		free(list);
		return FALSE;
	}

	loop = malloc(sizeof(struct loop_t));
	if(!loop)
	{
		free(dev);
		free(list);
		return FALSE;
	}

	while(1)
	{
		snprintf((char *)loop->name, 32, (const char *)"loop%d", i++);
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
	rem = size % SZ_512;
	size = size / SZ_512;
	if(rem > 0)
		size++;

	loop->busy 		= FALSE;
	loop->read_only	= FALSE;

	dev->name		= loop->name;
	dev->blksz		= SZ_512;
	dev->blkcnt		= size;
	dev->open 		= loop_open;
	dev->read		= loop_read;
	dev->write		= loop_write;
	dev->close		= loop_close;
	dev->priv		= loop;

	list->loop 	= loop;

	if(!register_block(dev))
	{
		free(loop);
		free(dev);
		free(list);
		return FALSE;
	}

	list_add(&list->entry, &loop_list->entry);

	return TRUE;
}

bool_t unregister_loop(const char * file)
{
	struct loop_list_t * list;
	struct list_head * pos;
	struct block_t * dev;
	char buf[MAX_PATH];

	if(!file)
		return FALSE;

	if(vfs_path_conv(file, buf) !=0)
		return FALSE;

	for(pos = (&loop_list->entry)->next; pos != (&loop_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct loop_list_t, entry);
		if(strcmp((char*)list->loop->path, (const char *)buf) == 0)
		{
			dev = search_block(list->loop->name);
			if(dev)
			{
				if(unregister_block(list->loop->name))
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

static s32_t loop_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct loop_list_t * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[loop]");

	for(pos = (&loop_list->entry)->next; pos != (&loop_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct loop_list_t, entry);
		len += sprintf((char *)(p + len), (const char *)"\r\n %s %s ", list->loop->name, list->loop->path);
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

static struct proc_t loop_proc = {
	.name	= "loop",
	.read	= loop_proc_read,
};

static __init void loop_pure_sync_init(void)
{
	proc_register(&loop_proc);
}

static __exit void loop_pure_sync_exit(void)
{
	proc_unregister(&loop_proc);
}

pure_initcall_sync(loop_pure_sync_init);
pure_exitcall_sync(loop_pure_sync_exit);
