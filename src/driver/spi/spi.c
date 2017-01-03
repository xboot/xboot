/*
 * driver/spi/spi.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <spi/spi.h>

struct spi_t * search_spi(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_SPI);
	if(!dev)
		return NULL;

	return (struct spi_t *)dev->priv;
}

bool_t register_spi(struct device_t ** device, struct spi_t * spi)
{
	struct device_t * dev;

	if(!spi || !spi->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(spi->name);
	dev->type = DEVICE_TYPE_SPI;
	dev->priv = spi;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_spi(struct spi_t * spi)
{
	struct device_t * dev;

	if(!spi || !spi->name)
		return FALSE;

	dev = search_device(spi->name, DEVICE_TYPE_SPI);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
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

	spi = search_spi(spibus);
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
