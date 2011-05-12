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
#include <charset.h>
#include <malloc.h>
#include <xml.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <fs/fsapi.h>
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
		if(strcmp(list->console->name, name) == 0)
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

	if(!console->name || (!console->getcode && !console->putcode) || search_console(console->name))
	{
		free(list);
		return FALSE;
	}

	list->console = console;
	list_add(&list->entry, &console_list->entry);

	if((console_stdin == NULL) && (console->getcode))
		console_stdin = console;

	if((console_stdout == NULL) && (console->putcode))
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

inline struct console * get_stdin(void)
{
	return console_stdin;
}

inline struct console * get_stdout(void)
{
	return console_stdout;
}

x_bool console_stdio_set(const char * in, const char * out)
{
	struct console * stdin, * stdout;

	if(!in || !out)
		return FALSE;

	stdin = search_console(in);
	if(!stdin || !stdin->getcode)
		return FALSE;

	stdout = search_console(out);
	if(!stdout || !stdout->putcode)
		return FALSE;

	console_stdin = stdin;
	console_stdout = stdout;

	return TRUE;
}

x_bool console_stdio_load(char * file)
{
	struct xml * root, * stdin, * stdout;

	root = xml_parse_file(file);
	if(!root || !root->name)
		return FALSE;

	if(strcmp(root->name, "console") != 0)
	{
		xml_free(root);
		return FALSE;
	}

	stdin = xml_get(root, "stdin", -1);
	if(! stdin)
	{
		xml_free(root);
		return FALSE;
	}

	stdout = xml_get(root, "stdout", -1);
	if(! stdout)
	{
		xml_free(root);
		return FALSE;
	}

	if(! console_stdio_set(xml_attr(stdin, "name"), xml_attr(stdout, "name")))
	{
		xml_free(root);
		return FALSE;
	}

	xml_free(root);
	return TRUE;
}

x_bool console_stdio_save(char * file)
{
	struct xml * root;
	struct xml * stdin, * stdout;
	char * str;
	x_s32 fd;

	root = xml_new("console");
	if(!root)
		return FALSE;

	stdin = xml_add_child(root, "stdin", 0);
	if(!stdin)
	{
		xml_free(root);
		return FALSE;
	}

	if(get_stdin() && get_stdin()->name)
		xml_set_attr(stdin, "name", get_stdin()->name);

	stdout = xml_add_child(root, "stdout", 1);
	if(!stdout)
	{
		xml_free(root);
		return FALSE;
	}

	if(get_stdout() && get_stdout()->name)
		xml_set_attr(stdout, "name", get_stdout()->name);

	str = xml_toxml(root);
	if(!str)
	{
		xml_free(root);
		return FALSE;
	}

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		free(str);
		xml_free(root);
		return FALSE;
	}

	write(fd, str, strlen((const char *)str));
	close(fd);

	free(str);
	xml_free(root);

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

x_bool console_getcursor(struct console * console)
{
	if(console && console->getcursor)
		return console->getcursor(console);
	return FALSE;
}

x_bool console_setcolor(struct console * console, enum tcolor f, enum tcolor b)
{
	if(console && console->setcolor)
		return console->setcolor(console, f, b);
	return FALSE;
}

x_bool console_getcolor(struct console * console, enum tcolor * f, enum tcolor * b)
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

x_bool console_getcode(struct console * console, x_u32 * code)
{
	if(console && console->getcode)
		return console->getcode(console, code);
	return FALSE;
}

x_bool console_putcode(struct console * console, x_u32 code)
{
	if(console && console->putcode)
		return console->putcode(console, code);
	return FALSE;
}

x_s32 console_print(struct console * console, const char * fmt, ...)
{
	va_list args;
	x_u32 code;
	x_s32 i;
	x_s8 *p, *buf;

	if(!console || !console->putcode)
		return 0;

	buf = malloc(SZ_4K);
	if(!buf)
		return 0;

	va_start(args, fmt);
	i = vsnprintf((x_s8 *)buf, SZ_4K, (x_s8 *)fmt, args);
	va_end(args);

	for(p = buf; utf8_to_ucs4(&code, 1, p, -1, (const x_s8 **)&p) > 0; )
	{
		console->putcode(console, code);
	}

	free(buf);
	return i;
}

x_bool console_hline(struct console * console, x_u32 code, x_u32 x0, x_u32 y0, x_u32 x)
{
	x_s32 w, h;
	x_s32 i;

	if(console && console->putcode)
	{
		if(!console_getwh(console, &w, &h))
			return FALSE;

		if(x0 >= w || y0 >= h)
			return FALSE;

		if(x0 + x >= w)
			x = w - x0;

		if(!console_gotoxy(console, x0, y0))
			return FALSE;

		for(i = 0; i < x; i++)
			console->putcode(console, code);

		return TRUE;
	}

	return FALSE;
}

x_bool console_vline(struct console * console, x_u32 code, x_u32 x0, x_u32 y0, x_u32 y)
{
	x_s32 w, h;
	x_s32 i;

	if(console && console->putcode)
	{
		if(!console_getwh(console, &w, &h))
			return FALSE;

		if(x0 >= w || y0 >= h)
			return FALSE;

		if(y0 + y >= h)
			y = h - y0;

		for(i = 0; i < y; i++)
		{
			console->gotoxy(console, x0, y0 + i);
			console->putcode(console, code);
		}

		return TRUE;
	}

	return FALSE;
}

x_bool console_rect(struct console * console, x_u32 hline, x_u32 vline, x_u32 lt, x_u32 rt, x_u32 lb, x_u32 rb, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_s32 width, height;

	if(console && console->putcode)
	{
		if(!console_getwh(console, &width, &height))
			return FALSE;

		if(x < 0 || y < 0 || w < 2 || h < 2)
			return FALSE;

		if(x >= width || y >= height)
			return FALSE;

		if(x + w - 1 >= width)
			return FALSE;

		if(y + h - 1 >= height)
			return FALSE;

		console_gotoxy(console, x, y);
		console_putcode(console, lt);

		console_gotoxy(console, x + w - 1, y);
		console_putcode(console, rt);

		console_gotoxy(console, x, y + h - 1);
		console_putcode(console, lb);

		console_gotoxy(console, x + w - 1, y + h - 1);
		console_putcode(console, rb);

		console_hline(console, hline, x + 1, y, w - 1 - 1);
		console_hline(console, hline, x + 1, y + h - 1, w - 1 - 1);
		console_vline(console, vline, x, y + 1, h - 1 - 1);
		console_vline(console, vline, x + w - 1, y + 1, h - 1 - 1);

		return TRUE;
	}

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

	len += sprintf((char *)(p + len), (const char *)"[standard console]");
	if(console_stdin)
		len += sprintf((char *)(p + len), (const char *)"\r\n stdin = %s", console_stdin->name);
	else
		len += sprintf((char *)(p + len), (const char *)"\r\n stdin = %s", "<NULL>");

	if(console_stdout)
		len += sprintf((char *)(p + len), (const char *)"\r\n stdout = %s", console_stdout->name);
	else
		len += sprintf((char *)(p + len), (const char *)"\r\n stdout = %s", "<NULL>");

	len += sprintf((char *)(p + len), (const char *)"\r\n[available console]");
	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		if(list->console->getcode && list->console->putcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "in,out");
		else if(list->console->getcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "in");
		else if(list->console->putcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "out");
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
