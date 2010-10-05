/*
 * xboot/kernel/core/resource.c
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
#include <types.h>
#include <string.h>
#include <macros.h>
#include <malloc.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/resource.h>


/* the list of resource */
static struct resource_list __resource_list = {
	.entry = {
		.next	= &(__resource_list.entry),
		.prev	= &(__resource_list.entry),
	},
};
struct resource_list * resource_list = &__resource_list;

/*
 * search resource by name
 */
static struct resource * search_resource(const char * name)
{
	struct resource_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&resource_list->entry)->next; pos != (&resource_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct resource_list, entry);
		if(strcmp((x_s8*)list->res->name, (const x_s8 *)name) == 0)
			return list->res;
	}

	return NULL;
}

/*
 * register a resource into resource_list
 */
x_bool register_resource(struct resource * res)
{
	struct resource_list * list;

	list = malloc(sizeof(struct resource_list));
	if(!list || !res)
	{
		free(list);
		return FALSE;
	}

	if(!res->name || search_resource(res->name))
	{
		free(list);
		return FALSE;
	}

	list->res = res;
	list_add(&list->entry, &resource_list->entry);

	return TRUE;
}

/*
 * unregister resource from resource_list
 */
x_bool unregister_resource(struct resource * res)
{
	struct resource_list * list;
	struct list_head * pos;

	if(!res || !res->name)
		return FALSE;

	for(pos = (&resource_list->entry)->next; pos != (&resource_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct resource_list, entry);
		if(list->res == res)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get resource data
 */
void * resource_get_data(const char * name)
{
	struct resource * res;

	res = search_resource(name);

	if(!res)
		return NULL;

	return res->data;
}

/*
 * resource proc interface
 */
static x_s32 resource_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct resource_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[resource]");
	for(pos = (&resource_list->entry)->next; pos != (&resource_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct resource_list, entry);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", list->res->name);
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

static struct proc resource_proc = {
	.name	= "resource",
	.read	= resource_proc_read,
};

/*
 * resource pure sync init
 */
static __init void resource_pure_sync_init(void)
{
	/* register resource proc interface */
	proc_register(&resource_proc);
}

static __exit void resource_pure_sync_exit(void)
{
	/* unregister resource proc interface */
	proc_unregister(&resource_proc);
}

module_init(resource_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(resource_pure_sync_exit, LEVEL_PURE_SYNC);
