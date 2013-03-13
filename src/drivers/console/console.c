/*
 * drivers/console/console.c
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
#include <xml.h>
#include <console/console.h>

extern void led_console_trigger_activity(void);

static struct console * console_stdin = NULL;
static struct console * console_stdout = NULL;
static struct console * console_stderr = NULL;

static struct console_list __console_list = {
	.entry = {
		.next	= &(__console_list.entry),
		.prev	= &(__console_list.entry),
	},
};
static struct console_list * console_list = &__console_list;

inline struct console * get_console_stdin(void)
{
	return console_stdin;
}

inline struct console * get_console_stdout(void)
{
	return console_stdout;
}

inline struct console * get_console_stderr(void)
{
	return console_stderr;
}

bool_t console_stdin_getcode(u32_t * code)
{
	if(!console_stdin || !console_stdin->getcode)
		return FALSE;

	return console_stdin->getcode(console_stdin, code);
}

bool_t console_stdin_getcode_with_timeout(u32_t * code, u32_t timeout)
{
	u32_t end;

	if(!console_stdin || !console_stdin->getcode)
		return FALSE;

	if(get_system_hz() > 0)
	{
		end = jiffies + timeout * get_system_hz() / 1000;

		while(! console_stdin->getcode(console_stdin, code))
		{
			if(jiffies >= end)
				return FALSE;
		}

		return TRUE;
	}
	else
	{
		end = timeout * 100;

		while(! console_stdin->getcode(console_stdin, code))
		{
			if(end <= 0)
				return FALSE;
			end--;
		}

		return TRUE;
	}

	return FALSE;
}

bool_t console_stdout_putc(char c)
{
	static size_t size = 0;
	static char buf[6];
	char * rest;
	u32_t code;

	if(!console_stdout || !console_stdout->putcode)
		return FALSE;

	buf[size++] = c;
	while(utf8_to_ucs4(&code, 1, buf, size, (const char **)&rest) > 0)
	{
		led_console_trigger_activity();

		size -= rest - buf;
		memmove(buf, rest, size);
		console_stdout->putcode(console_stdout, code);
	}
	return TRUE;
}

bool_t console_stderr_putc(char c)
{
	static size_t size = 0;
	static char buf[6];
	char * rest;
	u32_t code;

	if(!console_stderr || !console_stderr->putcode)
		return FALSE;

	buf[size++] = c;
	while(utf8_to_ucs4(&code, 1, buf, size, (const char **)&rest) > 0)
	{
		led_console_trigger_activity();

		size -= rest - buf;
		memmove(buf, rest, size);
		console_stderr->putcode(console_stderr, code);
	}
	return TRUE;
}

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

bool_t register_console(struct console * console)
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

	if((console_stderr == NULL) && (console->putcode))
		console_stderr = console;

	return TRUE;
}

bool_t unregister_console(struct console * console)
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

			if(console_stderr == console)
				console_stderr = NULL;

			return TRUE;
		}
	}

	return FALSE;
}

bool_t console_stdio_set(const char * in, const char * out, const char * err)
{
	struct console * cin, * cout, * cerr;

	if(!in || !out || !err)
		return FALSE;

	cin = search_console(in);
	if(!cin || !cin->getcode)
		return FALSE;

	cout = search_console(out);
	if(!cout || !cout->putcode)
		return FALSE;

	cerr = search_console(err);
	if(!cerr || !cerr->putcode)
		return FALSE;

	console_stdin = cin;
	console_stdout = cout;
	console_stderr = cerr;

	return TRUE;
}

bool_t console_stdio_load(char * file)
{
	struct xml * root;
	struct xml * in, * out, * err;

	root = xml_parse_file(file);
	if(!root || !root->name)
		return FALSE;

	if(strcmp(root->name, "console") != 0)
	{
		xml_free(root);
		return FALSE;
	}

	in = xml_get(root, "stdin", -1);
	if(! in)
	{
		xml_free(root);
		return FALSE;
	}

	out = xml_get(root, "stdout", -1);
	if(! out)
	{
		xml_free(root);
		return FALSE;
	}

	err = xml_get(root, "stderr", -1);
	if(! err)
	{
		xml_free(root);
		return FALSE;
	}

	if(! console_stdio_set(xml_attr(in, "name"), xml_attr(out, "name"), xml_attr(err, "name")))
	{
		xml_free(root);
		return FALSE;
	}

	xml_free(root);
	return TRUE;
}

bool_t console_stdio_save(char * file)
{
	struct xml * root;
	struct xml * in, * out, * err;
	char * str;
	s32_t fd;

	root = xml_new("console");
	if(!root)
		return FALSE;

	in = xml_add_child(root, "stdin", 0);
	if(!in)
	{
		xml_free(root);
		return FALSE;
	}

	if(console_stdin && console_stdin->name)
		xml_set_attr(in, "name", console_stdin->name);

	out = xml_add_child(root, "stdout", 1);
	if(!out)
	{
		xml_free(root);
		return FALSE;
	}

	if(console_stdout && console_stdout->name)
		xml_set_attr(out, "name", console_stdout->name);

	err = xml_add_child(root, "stderr", 1);
	if(!err)
	{
		xml_free(root);
		return FALSE;
	}

	if(console_stderr && console_stderr->name)
		xml_set_attr(err, "name", console_stderr->name);

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

bool_t console_getwh(struct console * console, s32_t * w, s32_t * h)
{
	if(console && console->getwh)
		return console->getwh(console, w, h);
	return FALSE;
}

bool_t console_getxy(struct console * console, s32_t * x, s32_t * y)
{
	if(console && console->getxy)
		return console->getxy(console, x, y);
	return FALSE;
}

bool_t console_gotoxy(struct console * console, s32_t x, s32_t y)
{
	if(console && console->gotoxy)
		return console->gotoxy(console, x, y);
	return FALSE;
}

bool_t console_setcursor(struct console * console, bool_t on)
{
	if(console && console->setcursor)
		return console->setcursor(console, on);
	return FALSE;
}

bool_t console_getcursor(struct console * console)
{
	if(console && console->getcursor)
		return console->getcursor(console);
	return FALSE;
}

bool_t console_setcolor(struct console * console, enum tcolor f, enum tcolor b)
{
	if(console && console->setcolor)
		return console->setcolor(console, f, b);
	return FALSE;
}

bool_t console_getcolor(struct console * console, enum tcolor * f, enum tcolor * b)
{
	if(console && console->getcolor)
		return console->getcolor(console, f, b);
	return FALSE;
}

bool_t console_cls(struct console * console)
{
	if(console && console->cls)
		return console->cls(console);
	return FALSE;
}

bool_t console_getcode(struct console * console, u32_t * code)
{
	if(console && console->getcode)
		return console->getcode(console, code);
	return FALSE;
}

bool_t console_putcode(struct console * console, u32_t code)
{
	if(console && console->putcode)
		return console->putcode(console, code);
	return FALSE;
}

bool_t console_onoff(struct console * console, bool_t flag)
{
	if(console && console->onoff)
		return console->onoff(console, flag);
	return FALSE;
}

int console_print(struct console * console, const char * fmt, ...)
{
	va_list ap;
	u32_t code;
	char *p, *buf;
	int len;

	if(!console || !console->putcode)
		return 0;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	if(len < 0)
		return 0;
	buf = malloc(len + 1);
	if(!buf)
		return 0;
	len = vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);

	for(p = buf; utf8_to_ucs4(&code, 1, p, -1, (const char **)&p) > 0; )
	{
		console->putcode(console, code);
	}

	free(buf);
	return len;
}

bool_t console_hline(struct console * console, u32_t code, u32_t x0, u32_t y0, u32_t x)
{
	s32_t w, h;
	s32_t i;

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

bool_t console_vline(struct console * console, u32_t code, u32_t x0, u32_t y0, u32_t y)
{
	s32_t w, h;
	s32_t i;

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

bool_t console_rect(struct console * console, u32_t hline, u32_t vline, u32_t lt, u32_t rt, u32_t lb, u32_t rb, u32_t x, u32_t y, u32_t w, u32_t h)
{
	s32_t width, height;

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

static s32_t console_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct console_list * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;

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

	if(console_stderr)
		len += sprintf((char *)(p + len), (const char *)"\r\n stderr = %s", console_stderr->name);
	else
		len += sprintf((char *)(p + len), (const char *)"\r\n stderr = %s", "<NULL>");

	len += sprintf((char *)(p + len), (const char *)"\r\n[available console]");
	for(pos = (&console_list->entry)->next; pos != (&console_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct console_list, entry);
		if(list->console->getcode && list->console->putcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "in,out,err");
		else if(list->console->getcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "in");
		else if(list->console->putcode)
			len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%s", list->console->name, (int)(16 - strlen((char *)list->console->name)), "", "out,err");
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

pure_initcall_sync(console_pure_sync_init);
pure_exitcall_sync(console_pure_sync_exit);
