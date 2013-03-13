/*
 * drivers/input/mouse/mouse.c
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
#include <hash.h>
#include <fifo.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <time/timer.h>
#include <input/input.h>
#include <input/mouse/mouse.h>

struct handler_onmouseraw_list
{
	handler_onmouseraw handler;
	struct list_head entry;
};

static struct handler_onmouseraw_list __handler_onmouseraw_list = {
	.entry = {
		.next	= &(__handler_onmouseraw_list.entry),
		.prev	= &(__handler_onmouseraw_list.entry),
	},
};
static struct handler_onmouseraw_list * handler_onmouseraw_list = &__handler_onmouseraw_list;

bool_t install_listener_onmouseraw(handler_onmouseraw raw)
{
	struct handler_onmouseraw_list * list;
	struct list_head * pos;

	if(!raw)
		return FALSE;

	for(pos = (&handler_onmouseraw_list->entry)->next; pos != (&handler_onmouseraw_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onmouseraw_list, entry);
		if(list->handler == raw)
			return FALSE;
	}

	list = malloc(sizeof(struct handler_onmouseraw_list));
	if(!list)
		return FALSE;

	list->handler = raw;
	list_add_tail(&list->entry, &handler_onmouseraw_list->entry);

	return TRUE;
}

bool_t remove_listener_onmouseraw(handler_onmouseraw raw)
{
	struct handler_onmouseraw_list * list;
	struct list_head * pos;

	if(!raw)
		return FALSE;

	for(pos = (&handler_onmouseraw_list->entry)->next; pos != (&handler_onmouseraw_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onmouseraw_list, entry);
		if(list->handler == raw)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

void mouse_input_handler(struct input_event * event)
{
	struct handler_onmouseraw_list * raw;
	struct list_head * pos;

	for(pos = (&handler_onmouseraw_list->entry)->next; pos != (&handler_onmouseraw_list->entry); pos = pos->next)
	{
		raw = list_entry(pos, struct handler_onmouseraw_list, entry);
		raw->handler(event);
	}
}
