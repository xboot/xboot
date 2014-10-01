/*
 * driver/console/console-uart.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <console/console.h>
#include <console/console-uart.h>

struct console_uart_data_t {
	struct uart_t * uart;
};

static ssize_t console_uart_read(struct console_t * console, unsigned char * buf, size_t count)
{
	struct console_uart_data_t * dat = (struct console_uart_data_t *)console->priv;
	return dat->uart->read(dat->uart, (u8_t *)buf, count);
}

static ssize_t console_uart_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	struct console_uart_data_t * dat = (struct console_uart_data_t *)console->priv;
	return dat->uart->write(dat->uart, (const u8_t *)buf, count);
}

static void console_uart_suspend(struct console_t * console)
{
}

static void console_uart_resume(struct console_t * console)
{
}

bool_t register_console_uart(struct uart_t * uart)
{
	struct console_uart_data_t * dat;
	struct console_t * console;

	if(!uart || !uart->name)
		return FALSE;

	if(!uart->read || !uart->write)
		return FALSE;

	dat = malloc(sizeof(struct console_uart_data_t));
	if(!dat)
		return FALSE;

	console = malloc(sizeof(struct console_t));
	if(!console)
	{
		free(dat);
		return FALSE;
	}

	dat->uart = uart;
	console->name = strdup(uart->name);
	console->read = console_uart_read,
	console->write = console_uart_write,
	console->suspend = console_uart_suspend,
	console->resume	= console_uart_resume,
	console->priv = dat;

	if(register_console(console))
		return TRUE;

	free(console->priv);
	free(console->name);
	free(console);
	return FALSE;
}

bool_t unregister_console_uart(struct uart_t * uart)
{
	struct console_t * console;

	console = search_console(uart->name);
	if(!console)
		return FALSE;

	if(!unregister_console(console))
		return FALSE;

	free(console->priv);
	free(console->name);
	free(console);
	return TRUE;
}
