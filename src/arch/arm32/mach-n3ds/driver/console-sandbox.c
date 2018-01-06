/*
 * driver/console-sandbox.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <console/console.h>
#include <sandbox.h>

static ssize_t console_sandbox_read(struct console_t * console, unsigned char * buf, size_t count)
{
	return 0;//sandbox_stdio_read((void *)buf, count);
}

static ssize_t console_sandbox_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	return 0;//sandbox_stdio_write((void *)buf, count);
}

static struct device_t * console_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct console_t * console;
	struct device_t * dev;

	console = malloc(sizeof(struct console_t));
	if(!console)
		return NULL;

	console->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	console->read = console_sandbox_read,
	console->write = console_sandbox_write,
	console->priv = NULL;

	if(!register_console(&dev, console))
	{
		free_device_name(console->name);
		free(console);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void console_sandbox_remove(struct device_t * dev)
{
	struct console_t * console = (struct console_t *)dev->priv;

	if(console && unregister_console(console))
	{
		free_device_name(console->name);
		free(console);
	}
}

static void console_sandbox_suspend(struct device_t * dev)
{
}

static void console_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t console_sandbox = {
	.name		= "console-sandbox",
	.probe		= console_sandbox_probe,
	.remove		= console_sandbox_remove,
	.suspend	= console_sandbox_suspend,
	.resume		= console_sandbox_resume,
};

static __init void console_sandbox_driver_init(void)
{
	register_driver(&console_sandbox);
}

static __exit void console_sandbox_driver_exit(void)
{
	unregister_driver(&console_sandbox);
}

driver_initcall(console_sandbox_driver_init);
driver_exitcall(console_sandbox_driver_exit);
