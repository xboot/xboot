/*
 * drivers/bus/spi/spi.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <bus/spi.h>

struct spi_t * search_bus_spi(const char * name)
{
	struct bus_t * bus;

	bus = search_bus_with_type(name, BUS_TYPE_SPI);
	if(!bus)
		return NULL;

	return (struct spi_t *)bus->driver;
}

bool_t register_bus_spi(struct spi_t * spi)
{
	struct bus_t * bus;

	if(!spi || !spi->name)
		return FALSE;

	bus = malloc(sizeof(struct bus_t));
	if(!bus)
		return FALSE;

	bus->name = strdup(spi->name);
	bus->type = BUS_TYPE_SPI;
	bus->driver = spi;
	bus->kobj = kobj_alloc_directory(bus->name);

	if(!register_bus(bus))
	{
		kobj_remove_self(bus->kobj);
		free(bus->name);
		free(bus);
		return FALSE;
	}

	if(spi->init)
		(spi->init)(spi);

	return TRUE;
}

bool_t unregister_bus_spi(struct spi_t * spi)
{
	struct bus_t * bus;
	struct spi_t * driver;

	if(!spi || !spi->name)
		return FALSE;

	bus = search_bus_with_type(spi->name, BUS_TYPE_SPI);
	if(!bus)
		return FALSE;

	driver = (struct spi_t *)(bus->driver);
	if(driver && driver->exit)
		(driver->exit)(spi);

	if(!unregister_bus(bus))
		return FALSE;

	kobj_remove_self(bus->kobj);
	free(bus->name);
	free(bus);

	return TRUE;
}
