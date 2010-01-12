/*
 * kernel/core/chrdev.c
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
#include <xboot/chrdev.h>


/* the list of char device */
static struct chrdev_list __chrdev_list = {
	.entry = {
		.next	= &(__chrdev_list.entry),
		.prev	= &(__chrdev_list.entry),
	},
};
struct chrdev_list * chrdev_list = &__chrdev_list;

/*
 * search char device by name
 */
struct chrdev * search_chrdev(const char * name)
{
	struct chrdev_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&chrdev_list->entry)->next; pos != (&chrdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct chrdev_list, entry);
		if(strcmp((x_s8*)list->dev->name, (const x_s8 *)name) == 0)
			return list->dev;
	}

	return NULL;
}

/*
 * search char device by name and device type
 */
struct chrdev * search_chrdev_with_type(const char * name, enum chrdev_type type)
{
	struct chrdev_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&chrdev_list->entry)->next; pos != (&chrdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct chrdev_list, entry);
		if( (list->dev->type == type) && (strcmp((x_s8*)list->dev->name, (const x_s8 *)name) == 0) )
		{
			return list->dev;
		}
	}

	return NULL;
}

/*
 * register a char device into chrdev_list
 */
x_bool register_chrdev(struct chrdev * dev)
{
	struct chrdev_list * list;

	list = malloc(sizeof(struct chrdev_list));
	if(!list || !dev)
	{
		free(list);
		return FALSE;
	}

	if(!dev->name || search_chrdev(dev->name))
	{
		free(list);
		return FALSE;
	}

	list->dev = dev;
	list_add(&list->entry, &chrdev_list->entry);

	return TRUE;
}

/*
 * unregister char device from chrdev_list
 */
x_bool unregister_chrdev(const char * name)
{
	struct chrdev_list * list;
	struct list_head * pos;

	if(!name)
		return FALSE;

	for(pos = (&chrdev_list->entry)->next; pos != (&chrdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct chrdev_list, entry);
		if(strcmp((x_s8*)list->dev->name, (const x_s8 *)name) == 0)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * char device proc interface
 */
static x_s32 chrdev_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct chrdev_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[char device]");

	for(pos = (&chrdev_list->entry)->next; pos != (&chrdev_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct chrdev_list, entry);
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

static struct proc chrdev_proc = {
	.name	= "chrdev",
	.read	= chrdev_proc_read,
};

/*
 * char device pure sync init
 */
static __init void chrdev_pure_sync_init(void)
{
	/* register char device proc interface */
	proc_register(&chrdev_proc);
}

static __exit void chrdev_pure_sync_exit(void)
{
	/* unregister char device proc interface */
	proc_unregister(&chrdev_proc);
}

module_init(chrdev_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(chrdev_pure_sync_exit, LEVEL_PURE_SYNC);
