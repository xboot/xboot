/*
 * drivers/input/keyboard/keyboard.c
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
#include <input/keyboard/keyboard.h>


struct handler_onkeyraw_list
{
	handler_onkeyraw handler;
	struct list_head entry;
};

struct handler_onkeyup_list
{
	handler_onkeyup handler;
	struct list_head entry;
};

struct handler_onkeydown_list
{
	handler_onkeydown handler;
	struct list_head entry;
};


static struct handler_onkeyraw_list __handler_onkeyraw_list = {
	.entry = {
		.next	= &(__handler_onkeyraw_list.entry),
		.prev	= &(__handler_onkeyraw_list.entry),
	},
};
static struct handler_onkeyraw_list * handler_onkeyraw_list = &__handler_onkeyraw_list;

static struct handler_onkeyup_list __handler_onkeyup_list = {
	.entry = {
		.next	= &(__handler_onkeyup_list.entry),
		.prev	= &(__handler_onkeyup_list.entry),
	},
};
static struct handler_onkeyup_list * handler_onkeyup_list = &__handler_onkeyup_list;

static struct handler_onkeydown_list __handler_onkeydown_list = {
	.entry = {
		.next	= &(__handler_onkeydown_list.entry),
		.prev	= &(__handler_onkeydown_list.entry),
	},
};
static struct handler_onkeydown_list * handler_onkeydown_list = &__handler_onkeydown_list;

bool_t install_listener_onkeyraw(handler_onkeyraw raw)
{
	struct handler_onkeyraw_list * list;
	struct list_head * pos;

	if(!raw)
		return FALSE;

	for(pos = (&handler_onkeyraw_list->entry)->next; pos != (&handler_onkeyraw_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeyraw_list, entry);
		if(list->handler == raw)
			return FALSE;
	}

	list = malloc(sizeof(struct handler_onkeyraw_list));
	if(!list)
		return FALSE;

	list->handler = raw;
	list_add_tail(&list->entry, &handler_onkeyraw_list->entry);

	return TRUE;
}

bool_t remove_listener_onkeyraw(handler_onkeyraw raw)
{
	struct handler_onkeyraw_list * list;
	struct list_head * pos;

	if(!raw)
		return FALSE;

	for(pos = (&handler_onkeyraw_list->entry)->next; pos != (&handler_onkeyraw_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeyraw_list, entry);
		if(list->handler == raw)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t install_listener_onkeyup(handler_onkeyup keyup)
{
	struct handler_onkeyup_list * list;
	struct list_head * pos;

	if(!keyup)
		return FALSE;

	for(pos = (&handler_onkeyup_list->entry)->next; pos != (&handler_onkeyup_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeyup_list, entry);
		if(list->handler == keyup)
			return FALSE;
	}

	list = malloc(sizeof(struct handler_onkeyup_list));
	if(!list)
		return FALSE;

	list->handler = keyup;
	list_add_tail(&list->entry, &handler_onkeyup_list->entry);

	return TRUE;
}

bool_t remove_listener_onkeyup(handler_onkeyup keyup)
{
	struct handler_onkeyup_list * list;
	struct list_head * pos;

	if(!keyup)
		return FALSE;

	for(pos = (&handler_onkeyup_list->entry)->next; pos != (&handler_onkeyup_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeyup_list, entry);
		if(list->handler == keyup)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t install_listener_onkeydown(handler_onkeydown keydown)
{
	struct handler_onkeydown_list * list;
	struct list_head * pos;

	if(!keydown)
		return FALSE;

	for(pos = (&handler_onkeydown_list->entry)->next; pos != (&handler_onkeydown_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeydown_list, entry);
		if(list->handler == keydown)
			return FALSE;
	}

	list = malloc(sizeof(struct handler_onkeydown_list));
	if(!list)
		return FALSE;

	list->handler = keydown;
	list_add_tail(&list->entry, &handler_onkeydown_list->entry);

	return TRUE;
}

bool_t remove_listener_onkeydown(handler_onkeydown keydown)
{
	struct handler_onkeydown_list * list;
	struct list_head * pos;

	if(!keydown)
		return FALSE;

	for(pos = (&handler_onkeydown_list->entry)->next; pos != (&handler_onkeydown_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct handler_onkeydown_list, entry);
		if(list->handler == keydown)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

void keyboard_input_handler(struct input_event * event)
{
	struct handler_onkeyraw_list * raw;
	struct handler_onkeyup_list * keyup;
	struct handler_onkeydown_list * keydown;
	struct list_head * pos;

	for(pos = (&handler_onkeyraw_list->entry)->next; pos != (&handler_onkeyraw_list->entry); pos = pos->next)
	{
		raw = list_entry(pos, struct handler_onkeyraw_list, entry);
		raw->handler(event);
	}

	switch(event->value)
	{
	case KEY_BUTTON_UP:
		for(pos = (&handler_onkeyup_list->entry)->next; pos != (&handler_onkeyup_list->entry); pos = pos->next)
		{
			keyup = list_entry(pos, struct handler_onkeyup_list, entry);
			keyup->handler(event->code);
		}
		break;

	case KEY_BUTTON_DOWN:
		for(pos = (&handler_onkeydown_list->entry)->next; pos != (&handler_onkeydown_list->entry); pos = pos->next)
		{
			keydown = list_entry(pos, struct handler_onkeydown_list, entry);
			keydown->handler(event->code);
		}
		break;

	default:
		break;
	}
}
