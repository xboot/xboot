/*
 * driver/dac-8563.c
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
#include <dac/dac.h>

struct dac_8563_pdata_t
{
	struct spi_device_t * dev;
};

static inline void dac8563_set(struct spi_device_t * dev, u8_t cmd, u16_t dat)
{
	u8_t tx[3];

	spi_device_select(dev);
	tx[0] = cmd;
	tx[1] = (dat >> 8) & 0xff;
	tx[2] = (dat >> 0) & 0xff;
	spi_device_write_then_read(dev, tx, 3, 0, 0);
	spi_device_deselect(dev);
}

static void dac_8563_write(struct dac_t * dac, int channel, u32_t value)
{
	struct dac_8563_pdata_t * pdat = (struct dac_8563_pdata_t *)dac->priv;
	dac8563_set(pdat->dev, (0x3 << 3) | (channel & 0x1), value & 0xffff);
}

static struct device_t * dac_8563_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dac_8563_pdata_t * pdat;
	struct dac_t * dac;
	struct device_t * dev;
	struct spi_device_t * spidev;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	pdat = malloc(sizeof(struct dac_8563_pdata_t));
	if(!pdat)
	{
		spi_device_free(spidev);
		return NULL;
	}

	dac = malloc(sizeof(struct dac_t));
	if(!dac)
	{
		free(pdat);
		return NULL;
	}

	pdat->dev = spidev;

	dac->name = alloc_device_name(dt_read_name(n), -1);
	dac->vreference = dt_read_int(n, "reference-voltage", 5000000);
	dac->resolution = 16;
	dac->nchannel = 2;
	dac->write = dac_8563_write;
	dac->priv = pdat;

	dac8563_set(pdat->dev, (0x4 << 3) | (0x0 << 0), 0x3);
	dac8563_set(pdat->dev, (0x6 << 3) | (0x0 << 0), 0x3);
	dac8563_set(pdat->dev, (0x3 << 3) | (0x0 << 0), 0x7fff);
	dac8563_set(pdat->dev, (0x3 << 3) | (0x1 << 0), 0x7fff);
	dac8563_set(pdat->dev, (0x7 << 3) | (0x0 << 0), 0x1);

	if(!register_dac(&dev, dac))
	{
		spi_device_free(pdat->dev);

		free_device_name(dac->name);
		free(dac->priv);
		free(dac);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void dac_8563_remove(struct device_t * dev)
{
	struct dac_t * dac = (struct dac_t *)dev->priv;
	struct dac_8563_pdata_t * pdat = (struct dac_8563_pdata_t *)dac->priv;

	if(dac && unregister_dac(dac))
	{
		spi_device_free(pdat->dev);

		free_device_name(dac->name);
		free(dac->priv);
		free(dac);
	}
}

static void dac_8563_suspend(struct device_t * dev)
{
}

static void dac_8563_resume(struct device_t * dev)
{
}

static struct driver_t dac_8563 = {
	.name		= "dac-8563",
	.probe		= dac_8563_probe,
	.remove		= dac_8563_remove,
	.suspend	= dac_8563_suspend,
	.resume		= dac_8563_resume,
};

static __init void dac_8563_driver_init(void)
{
	register_driver(&dac_8563);
}

static __exit void dac_8563_driver_exit(void)
{
	unregister_driver(&dac_8563);
}

driver_initcall(dac_8563_driver_init);
driver_exitcall(dac_8563_driver_exit);
