/*
 * drivers/console/console.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <malloc.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <console/console.h>

/*
 * the stdin and stdout console
 */
struct console * stdin = NULL;
struct console * stdout = NULL;

/* the list of console */
static struct console_list __console_list = {
	.entry = {
		.next	= &(__console_list.entry),
		.prev	= &(__console_list.entry),
	},
};
static struct console_list * console_list = &__console_list;

/*
 * search console by name
 */
struct console * search_console(const char *name)
{
	struct console_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		if(strcmp((x_s8*)list->console->name, (const x_s8 *)name) == 0)
			return list->console;
	}

	return NULL;
}

/*
 * register a console into console_list
 */
x_bool register_console(struct console * console)
{
	struct console_list * list;

	list = malloc(sizeof(struct console_list));
	if(!list || !console)
	{
		free(list);
		return FALSE;
	}

	if(!console->name || (!console->getchar && !console->putchar) || search_console(console->name))
	{
		free(list);
		return FALSE;
	}

	list->console = console;
	list_add(&list->entry, &console_list->entry);

	return TRUE;
}

/*
 * unregister console from console_list
 */
x_bool unregister_console(struct console * console)
{
	struct console_list * list;
	struct list_head * pos;

	if(!console || !console->name)
		return FALSE;

	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		if(list->console == console)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * console color to rgba
 */
x_bool console_color_to_rgba(enum console_color c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a)
{
	switch(c)
	{
	case CONSOLE_NONE:
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 0;
		break;

	case CONSOLE_BLACK:
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_RED:
		*r = 255;
		*g = 0;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_GREEN:
		*r = 0;
		*g = 255;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_YELLOW:
		*r = 255;
		*g = 255;
		*b = 0;
		*a = 255;
		break;

	case CONSOLE_BULE:
		*r = 0;
		*g = 0;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_MAGENTA:
		*r = 255;
		*g = 0;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_CYAN:
		*r = 0;
		*g = 255;
		*b = 255;
		*a = 255;
		break;

	case CONSOLE_WHITE:
		*r = 255;
		*g = 255;
		*b = 255;
		*a = 255;
		break;

	default:
		*r = 0;
		*g = 0;
		*b = 0;
		*a = 255;
		break;
	}

	return TRUE;
}

/*
 * console proc interface
 */
static x_s32 console_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct console_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[console]");
	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", list->console->name);
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

static struct proc console_proc = {
	.name	= "console",
	.read	= console_proc_read,
};

/*
 * console pure sync init
 */
static __init void console_pure_sync_init(void)
{
	/* register proc interface for console */
	proc_register(&console_proc);
}

static __exit void console_pure_sync_exit(void)
{
	/* unregister console proc interface */
	proc_unregister(&console_proc);
}

module_init(console_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(console_pure_sync_exit, LEVEL_PURE_SYNC);
