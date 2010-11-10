/*
 * drivers/console/console.c
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
#include <hash.h>
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <console/console.h>

static struct console * console_stdin = NULL;
static struct console * console_stdout = NULL;

static struct console_list __console_list = {
	.entry = {
		.next	= &(__console_list.entry),
		.prev	= &(__console_list.entry),
	},
};
static struct console_list * console_list = &__console_list;

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

	if((console_stdin == NULL) && (console->getchar))
		console_stdin = console;

	if((console_stdout == NULL) && (console->putchar))
		console_stdout = console;

	return TRUE;
}

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

			if(console_stdin == console)
				console_stdin = NULL;

			if(console_stdout == console)
				console_stdout = NULL;

			return TRUE;
		}
	}

	return FALSE;
}

struct console * get_stdin(void)
{
	return console_stdin;
}

struct console * get_stdout(void)
{
	return console_stdout;
}

x_bool set_stdinout(const char * in, const char * out)
{
	struct console * stdin, * stdout;

	if(!in || !out)
		return FALSE;

	stdin = search_console(in);
	if(!stdin || !stdin->getchar)
		return FALSE;

	stdout = search_console(out);
	if(!stdout || !stdout->putchar)
		return FALSE;

	console_stdin = stdin;
	console_stdout = stdout;

	return TRUE;
}

x_bool console_getwh(struct console * console, x_s32 * w, x_s32 * h)
{
	if(console && console->getwh)
		return console->getwh(console, w, h);
	return FALSE;
}

x_bool console_getxy(struct console * console, x_s32 * x, x_s32 * y)
{
	if(console && console->getxy)
		return console->getxy(console, x, y);
	return FALSE;
}

x_bool console_gotoxy(struct console * console, x_s32 x, x_s32 y)
{
	if(console && console->gotoxy)
		return console->gotoxy(console, x, y);
	return FALSE;
}

x_bool console_setcursor(struct console * console, x_bool on)
{
	if(console && console->setcursor)
		return console->setcursor(console, on);
	return FALSE;
}

x_bool console_setcolor(struct console * console, enum console_color f, enum console_color b)
{
	if(console && console->setcolor)
		return console->setcolor(console, f, b);
	return FALSE;
}

x_bool console_getcolor(struct console * console, enum console_color * f, enum console_color * b)
{
	if(console && console->getcolor)
		return console->getcolor(console, f, b);
	return FALSE;
}

x_bool console_cls(struct console * console)
{
	if(console && console->cls)
		return console->cls(console);
	return FALSE;
}

x_bool console_refresh(struct console * console)
{
	if(console && console->refresh)
		return console->refresh(console);
	return FALSE;
}

x_bool console_getchar(struct console * console, x_u32 * c)
{
	if(console && console->getchar)
		return console->getchar(console, c);
	return FALSE;
}

x_bool console_putchar(struct console * console, x_u32 c)
{
	if(console && console->putchar)
		return console->putchar(console, c);
	return FALSE;
}

static x_s32 console_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct console_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[standard console]");
	if(console_stdin)
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n stdin = %s", console_stdin->name);
	else
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n stdin = %s", "<NULL>");

	if(console_stdout)
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n stdout = %s", console_stdout->name);
	else
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n stdout = %s", "<NULL>");

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n[available console]");
	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		if(list->console->getchar && list->console->putchar)
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((x_s8 *)list->console->name)), "", "in,out");
		else if(list->console->getchar)
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((x_s8 *)list->console->name)), "", "in");
		else if(list->console->putchar)
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((x_s8 *)list->console->name)), "", "out");
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

static __init void console_pure_sync_init(void)
{
	proc_register(&console_proc);
}

static __exit void console_pure_sync_exit(void)
{
	proc_unregister(&console_proc);
}

module_init(console_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(console_pure_sync_exit, LEVEL_PURE_SYNC);
