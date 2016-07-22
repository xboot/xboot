/*
 * drivers/console/console.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <console/console-uart.h>
#include <console/console-fb.h>
#include <console/console-input.h>
#include <console/console.h>

static struct console_t * __console_stdin = NULL;
static struct console_t * __console_stdout = NULL;
static struct console_t * __console_stderr = NULL;

static inline ssize_t console_read(struct console_t * console, unsigned char * buf, size_t count)
{
	if(console && console->read)
		return console->read(console, buf, count);
	return 0;
}

static inline ssize_t console_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	if(console && console->write)
		return console->write(console, buf, count);
	return 0;
}

ssize_t console_stdin_read(unsigned char * buf, size_t count)
{
	return console_read(__console_stdin, buf, count);
}

ssize_t console_stdout_write(const unsigned char * buf, size_t count)
{
	return console_write(__console_stdout, buf, count);
}

ssize_t console_stderr_write(const unsigned char * buf, size_t count)
{
	return console_write(__console_stderr, buf, count);
}

struct console_t * search_console(const char * name)
{
	struct device_t * dev;
	char dname[64];

	snprintf(dname, sizeof(dname), "console.%s", name);
	dev = search_device_with_type(dname, DEVICE_TYPE_CONSOLE);
	if(!dev)
		return NULL;

	return (struct console_t *)dev->priv;
}

bool_t register_console(struct console_t * console)
{
	struct device_t * dev;
	char dname[64];

	if(!console || !console->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	snprintf(dname, sizeof(dname), "console.%s", console->name);
	dev->name = strdup(dname);
	dev->type = DEVICE_TYPE_CONSOLE;
	dev->driver = console;
	dev->priv = console;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_console(struct console_t * console)
{
	struct device_t * dev;
	char dname[64];

	if(!console || !console->name)
		return FALSE;

	snprintf(dname, sizeof(dname), "console.%s", console->name);
	dev = search_device_with_type(dname, DEVICE_TYPE_CONSOLE);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
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
	if(c && c->read)
		__console_stdin = c;

	c = search_console(dat->out);
	if(c && c->write)
		__console_stdout = c;

	c = search_console(dat->err);
	if(c && c->write)
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
	resource_for_each("console", console_stdio_register);
}

static __exit void console_stdio_device_exit(void)
{
	resource_for_each("console", console_stdio_unregister);
}

postdevice_initcall(console_stdio_device_init);
postdevice_exitcall(console_stdio_device_exit);
