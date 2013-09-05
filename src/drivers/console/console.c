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
#include <spinlock.h>
#include <console/console-uart.h>
#include <console/console-input.h>
#include <console/console-fb.h>
#include <console/console.h>

struct console_list_t {
	struct console_t * console;
	struct list_head entry;
};

static struct console_list_t __console_list = {
	.entry = {
		.next	= &(__console_list.entry),
		.prev	= &(__console_list.entry),
	},
};
static spinlock_t __console_list_lock = SPIN_LOCK_INIT();

static struct console_t * __console_stdin = NULL;
static struct console_t * __console_stdout = NULL;
static struct console_t * __console_stderr = NULL;

struct console_t * console_get_stdin(void)  { return __console_stdin;  }
struct console_t * console_get_stdout(void) { return __console_stdout; }
struct console_t * console_get_stderr(void) { return __console_stderr; }

bool_t console_stdin_getcode(u32_t * code)
{
	if(!__console_stdin || !__console_stdin->getcode)
		return FALSE;

	return __console_stdin->getcode(__console_stdin, code);
}

bool_t console_stdin_getcode_with_timeout(u32_t * code, u32_t timeout)
{
	u32_t end;

	if(!__console_stdin || !__console_stdin->getcode)
		return FALSE;

	if(HZ > 0)
	{
		end = jiffies + msecs_to_jiffies(timeout);

		while(! __console_stdin->getcode(__console_stdin, code))
		{
			if(time_after_eq(jiffies, end))
				return FALSE;
		}

		return TRUE;
	}
	else
	{
		end = timeout * 100;

		while(! __console_stdin->getcode(__console_stdin, code))
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

	if(!__console_stdout || !__console_stdout->putcode)
		return FALSE;

	buf[size++] = c;
	while(utf8_to_ucs4(&code, 1, buf, size, (const char **)&rest) > 0)
	{
		size -= rest - buf;
		memmove(buf, rest, size);
		__console_stdout->putcode(__console_stdout, code);
	}
	return TRUE;
}

bool_t console_stderr_putc(char c)
{
	static size_t size = 0;
	static char buf[6];
	char * rest;
	u32_t code;

	if(!__console_stderr || !__console_stderr->putcode)
		return FALSE;

	buf[size++] = c;
	while(utf8_to_ucs4(&code, 1, buf, size, (const char **)&rest) > 0)
	{
		size -= rest - buf;
		memmove(buf, rest, size);
		__console_stderr->putcode(__console_stderr, code);
	}
	return TRUE;
}

bool_t console_getwh(struct console_t * console, s32_t * w, s32_t * h)
{
	if(console && console->getwh)
		return console->getwh(console, w, h);
	return FALSE;
}

bool_t console_getxy(struct console_t * console, s32_t * x, s32_t * y)
{
	if(console && console->getxy)
		return console->getxy(console, x, y);
	return FALSE;
}

bool_t console_gotoxy(struct console_t * console, s32_t x, s32_t y)
{
	if(console && console->gotoxy)
		return console->gotoxy(console, x, y);
	return FALSE;
}

bool_t console_setcursor(struct console_t * console, bool_t on)
{
	if(console && console->setcursor)
		return console->setcursor(console, on);
	return FALSE;
}

bool_t console_getcursor(struct console_t * console)
{
	if(console && console->getcursor)
		return console->getcursor(console);
	return FALSE;
}

bool_t console_setcolor(struct console_t * console, enum tcolor_t f, enum tcolor_t b)
{
	if(console && console->setcolor)
		return console->setcolor(console, f, b);
	return FALSE;
}

bool_t console_getcolor(struct console_t * console, enum tcolor_t * f, enum tcolor_t * b)
{
	if(console && console->getcolor)
		return console->getcolor(console, f, b);
	return FALSE;
}

bool_t console_cls(struct console_t * console)
{
	if(console && console->cls)
		return console->cls(console);
	return FALSE;
}

bool_t console_getcode(struct console_t * console, u32_t * code)
{
	if(console && console->getcode)
		return console->getcode(console, code);
	return FALSE;
}

bool_t console_putcode(struct console_t * console, u32_t code)
{
	if(console && console->putcode)
		return console->putcode(console, code);
	return FALSE;
}

bool_t console_onoff(struct console_t * console, bool_t flag)
{
	if(console && console->onoff)
		return console->onoff(console, flag);
	return FALSE;
}

bool_t console_hline(struct console_t * console, u32_t code, u32_t x0, u32_t y0, u32_t x)
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

bool_t console_vline(struct console_t * console, u32_t code, u32_t x0, u32_t y0, u32_t y)
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

bool_t console_rect(struct console_t * console, u32_t hline, u32_t vline, u32_t lt, u32_t rt, u32_t lb, u32_t rb, u32_t x, u32_t y, u32_t w, u32_t h)
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

int console_print(struct console_t * console, const char * fmt, ...)
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

struct console_t * search_console(const char * name)
{
	struct console_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__console_list.entry), entry)
	{
		if(strcmp(pos->console->name, name) == 0)
			return pos->console;
	}

	return NULL;
}

bool_t register_console(struct console_t * console)
{
	struct console_list_t * cl;

	if(!console || !console->name)
		return FALSE;

	if(!console->getcode && !console->putcode)
		return FALSE;

	if(search_console(console->name))
		return FALSE;

	cl = malloc(sizeof(struct console_list_t));
	if(!cl)
		return FALSE;

	cl->console = console;

	spin_lock_irq(&__console_list_lock);
	list_add_tail(&cl->entry, &(__console_list.entry));
	spin_unlock_irq(&__console_list_lock);

	return TRUE;
}

bool_t unregister_console(struct console_t * console)
{
	struct console_list_t * pos, * n;

	if(!console || !console->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__console_list.entry), entry)
	{
		if(pos->console == console)
		{
			spin_lock_irq(&__console_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__console_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

static bool_t console_stdio_register(struct resource_t * res)
{
	struct console_stdio_data_t * dat = (struct console_stdio_data_t *)res->data;
	struct console_t * c;

	if(!search_console(dat->in))
	{
		if(register_console_uart(search_bus_uart(dat->in)))	{ }
		else if (register_console_framebuffer(search_framebuffer(dat->in))) { }
		else if (register_console_input(search_input(dat->in))) { }
	}

	if(!search_console(dat->out))
	{
		if(register_console_uart(search_bus_uart(dat->out))) { }
		else if (register_console_framebuffer(search_framebuffer(dat->out))) { }
		else if (register_console_input(search_input(dat->out))) { }
	}

	if(!search_console(dat->err))
	{
		if(register_console_uart(search_bus_uart(dat->err))) { }
		else if (register_console_framebuffer(search_framebuffer(dat->err))) { }
		else if (register_console_input(search_input(dat->err))) { }
	}

	c = search_console(dat->in);
	if(c && c->getcode)
		__console_stdin = c;

	c = search_console(dat->out);
	if(c && c->putcode)
		__console_stdout = c;

	c = search_console(dat->err);
	if(c && c->putcode)
		__console_stderr = c;

	LOG("Console stdio: [%s] [%s] [%s]",
			__console_stdin  ? __console_stdin->name  : "N/A",
			__console_stdout ? __console_stdout->name : "N/A",
			__console_stderr ? __console_stderr->name : "N/A");
	return TRUE;
}

static bool_t console_stdio_unregister(struct resource_t * res)
{
	struct console_stdio_data_t * dat = (struct console_stdio_data_t *)res->data;
	struct console_t * c;

	c = search_console(dat->in);
	if(c)
		unregister_console(c);

	c = search_console(dat->out);
	if(c)
		unregister_console(c);

	c = search_console(dat->err);
	if(c)
		unregister_console(c);

	__console_stdin = NULL;
	__console_stdout = NULL;
	__console_stderr = NULL;

	return TRUE;
}

static __init void console_stdio_device_init(void)
{
	resource_for_each_with_name("console", console_stdio_register);
}

static __exit void console_stdio_device_exit(void)
{
	resource_for_each_with_name("console", console_stdio_unregister);
}

device_sync_initcall(console_stdio_device_init);
device_sync_exitcall(console_stdio_device_exit);
