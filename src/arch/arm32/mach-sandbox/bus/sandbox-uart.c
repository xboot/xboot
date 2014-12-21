/*
 * bus/sandbox-uart.c
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

#include <xboot.h>
#include <bus/uart.h>
#include <sandbox.h>

static bool_t sandbox_uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	return TRUE;
}

static void sandbox_uart_init(struct uart_t * uart)
{
}

static void sandbox_uart_exit(struct uart_t * uart)
{
}

static ssize_t sandbox_uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	return sandbox_console_read((void *)buf, count);
}

static ssize_t sandbox_uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	return sandbox_console_write((void *)buf, count);
}

static bool_t sandbox_register_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart->name = strdup(name);
	uart->init = sandbox_uart_init;
	uart->exit = sandbox_uart_exit;
	uart->read = sandbox_uart_read;
	uart->write = sandbox_uart_write;
	uart->setup = sandbox_uart_setup;
	uart->priv = res;

	if(register_bus_uart(uart))
		return TRUE;

	free(uart->name);
	free(uart);
	return FALSE;
}

static bool_t sandbox_unregister_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;

	if(!unregister_bus_uart(uart))
		return FALSE;

	free(uart->name);
	free(uart);
	return TRUE;
}

static __init void sandbox_bus_uart_init(void)
{
	resource_for_each_with_name("sandbox-uart", sandbox_register_bus_uart);
}

static __exit void sandbox_bus_uart_exit(void)
{
	resource_for_each_with_name("sandbox-uart", sandbox_unregister_bus_uart);
}

bus_initcall(sandbox_bus_uart_init);
bus_exitcall(sandbox_bus_uart_exit);
