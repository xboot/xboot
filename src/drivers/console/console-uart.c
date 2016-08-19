/*
 * driver/console/console-uart.c
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

#include <xboot.h>
#include <uart/uart.h>
#include <console/console.h>

struct console_uart_pdata_t {
	struct uart_t * uart;
};

static ssize_t console_uart_read(struct console_t * console, unsigned char * buf, size_t count)
{
	struct console_uart_pdata_t * pdat = (struct console_uart_pdata_t *)console->priv;
	return pdat->uart->read(pdat->uart, (u8_t *)buf, count);
}

static ssize_t console_uart_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	struct console_uart_pdata_t * pdat = (struct console_uart_pdata_t *)console->priv;
	return pdat->uart->write(pdat->uart, (const u8_t *)buf, count);
}

static struct device_t * console_uart_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct console_uart_pdata_t * pdat;
	struct console_t * console;
	struct device_t * dev;
	struct uart_t * uart = search_uart(dt_read_string(n, "uart", NULL));

	if(!uart)
		return NULL;

	pdat = malloc(sizeof(struct console_uart_pdata_t));
	if(!pdat)
		return NULL;

	console = malloc(sizeof(struct console_t));
	if(!console)
	{
		free(pdat);
		return NULL;
	}

	pdat->uart = uart;

	console->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	console->read = console_uart_read,
	console->write = console_uart_write,
	console->priv = pdat;

	if(!register_console(&dev, console))
	{
		free_device_name(console->name);
		free(console->priv);
		free(console);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void console_uart_remove(struct device_t * dev)
{
	struct console_t * console = (struct console_t *)dev->priv;

	if(console && unregister_console(console))
	{
		free_device_name(console->name);
		free(console->priv);
		free(console);
	}
}

static void console_uart_suspend(struct device_t * dev)
{
}

static void console_uart_resume(struct device_t * dev)
{
}

struct driver_t console_uart = {
	.name		= "console-uart",
	.probe		= console_uart_probe,
	.remove		= console_uart_remove,
	.suspend	= console_uart_suspend,
	.resume		= console_uart_resume,
};

static __init void console_uart_driver_init(void)
{
	register_driver(&console_uart);
}

static __exit void console_uart_driver_exit(void)
{
	unregister_driver(&console_uart);
}

driver_initcall(console_uart_driver_init);
driver_exitcall(console_uart_driver_exit);
