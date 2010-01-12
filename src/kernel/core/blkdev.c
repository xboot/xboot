/*
 * kernel/core/blkdev.c
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
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/blkdev.h>


/* the list of block device */
static struct blkdev_list __blkdev_list = {
	.entry = {
		.next	= &(__blkdev_list.entry),
		.prev	= &(__blkdev_list.entry),
	},
};
struct blkdev_list * blkdev_list = &__blkdev_list;

/*
 * search block device by name
 */
struct blkdev * search_blkdev(const char * name)
{
	struct blkdev_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&blkdev_list->entry)->next; pos != (&blkdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkdev_list, entry);
		if(strcmp((x_s8*)list->dev->name, (const x_s8 *)name) == 0)
			return list->dev;
	}

	return NULL;
}

/*
 * register a block device into blkdev_list
 */
x_bool register_blkdev(struct blkdev * dev)
{
	struct blkdev_list * list;

	list = malloc(sizeof(struct blkdev_list));
	if(!list || !dev)
	{
		free(list);
		return FALSE;
	}

	if(!dev->name || search_blkdev(dev->name))
	{
		free(list);
		return FALSE;
	}

	list->dev = dev;
	list_add(&list->entry, &blkdev_list->entry);

	return TRUE;
}

/*
 * unregister block device from blkdev_list
 */
x_bool unregister_blkdev(struct blkdev * dev)
{
	struct blkdev_list * list;
	struct list_head * pos;

	if(!dev || !dev->name)
		return FALSE;

	for(pos = (&blkdev_list->entry)->next; pos != (&blkdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkdev_list, entry);
		if(list->dev == dev)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * blkdev proc interface
 */
static x_s32 blkdev_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct blkdev_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[block device]");

	for(pos = (&blkdev_list->entry)->next; pos != (&blkdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct blkdev_list, entry);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", list->dev->name);
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

static struct proc blkdev_proc = {
	.name	= "blkdev",
	.read	= blkdev_proc_read,
};

/*
 * blkdev pure sync init
 */
static __init void blkdev_pure_sync_init(void)
{
	/* register block device proc interface */
	proc_register(&blkdev_proc);
}

static __exit void blkdev_pure_sync_exit(void)
{
	/* unregister block device proc interface */
	proc_unregister(&blkdev_proc);
}

module_init(blkdev_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(blkdev_pure_sync_exit, LEVEL_PURE_SYNC);
