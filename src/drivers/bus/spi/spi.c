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

int spi_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	if(!spi || !msg)
		return 0;
	return spi->transfer(spi, msg);
}

void spi_chipselect(struct spi_t * spi, int enable)
{
	if(spi && spi->chipselect)
		spi->chipselect(spi, enable);
}

struct spi_device_t * spi_device_alloc(const char * spibus, int mode, int bits, int speed)
{
	struct spi_device_t * dev;
	struct spi_t * spi;

	spi = search_bus_spi(spibus);
	if(!spi)
		return NULL;

	dev = malloc(sizeof(struct spi_device_t));
	if(!dev)
		return NULL;

	dev->spi = spi;
	dev->mode = mode & 0x3;
	dev->bits = bits;
	dev->speed = (speed > 0) ? speed : 0;
	return dev;
}

void spi_device_free(struct spi_device_t * dev)
{
	if(dev)
		free(dev);
}

int spi_device_write_then_read(struct spi_device_t * dev, void * txbuf, int txlen, void * rxbuf, int rxlen)
{
	struct spi_msg_t msg;

	if(!dev)
		return -1;

	msg.mode = dev->mode;
	msg.bits = dev->bits;
	msg.speed = dev->speed;

	msg.txbuf = txbuf;
	msg.rxbuf = NULL;
	msg.len = txlen;
	if(dev->spi->transfer(dev->spi, &msg) != txlen)
		return -1;

	msg.txbuf = NULL;
	msg.rxbuf = rxbuf;
	msg.len = rxlen;
	if(dev->spi->transfer(dev->spi, &msg) != rxlen)
		return -1;

	return 0;
}

void spi_device_chipselect(struct spi_device_t * dev, int enable)
{
	if(dev && dev->spi)
		spi_chipselect(dev->spi, enable);
}
