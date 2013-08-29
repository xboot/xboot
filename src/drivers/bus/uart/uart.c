/*
 * drivers/bus/uart/uart.c
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
#include <bus/uart.h>

struct uart_t * search_bus_uart(const char * name)
{
	struct bus_t * bus;

	bus = search_bus_with_type(name, BUS_TYPE_UART);
	if(!bus)
		return NULL;

	return (struct uart_t *)bus->driver;
}

bool_t register_bus_uart(struct uart_t * uart)
{
	struct bus_t * bus;

	if(!uart || !uart->name)
		return FALSE;

	bus = malloc(sizeof(struct bus_t));
	if(!bus)
		return FALSE;

	bus->name = strdup(uart->name);
	bus->type = BUS_TYPE_UART;
	bus->driver = uart;
	bus->kobj = kobj_alloc_directory(bus->name);

	if(!register_bus(bus))
	{
		kobj_remove_self(bus->kobj);
		free(bus->name);
		free(bus);
		return FALSE;
	}

	if(uart->init)
		(uart->init)(uart);

	return TRUE;
}

bool_t unregister_bus_uart(struct uart_t * uart)
{
	struct bus_t * bus;
	struct uart_t * driver;

	if(!uart || !uart->name)
		return FALSE;

	bus = search_bus_with_type(uart->name, BUS_TYPE_UART);
	if(!bus)
		return FALSE;

	driver = (struct uart_t *)(bus->driver);
	if(driver && driver->exit)
		(driver->exit)(uart);

	if(!unregister_bus(bus))
		return FALSE;

	kobj_remove_self(bus->kobj);
	free(bus->name);
	free(bus);

	return TRUE;
}
