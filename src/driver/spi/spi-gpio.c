/*
 * driver/spi/spi-gpio.c
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
#include <gpio/gpio.h>
#include <spi/spi.h>

struct spi_gpio_pdata_t {
	int sclk_pin;
	int mosi_pin;
	int miso_pin;
	int cs_pin;
};

static inline void spi_gpio_setsclk(struct spi_gpio_pdata_t * pdat, int state)
{
	gpio_set_value(pdat->sclk_pin, state);
}

static inline void spi_gpio_setmosi(struct spi_gpio_pdata_t * pdat, int state)
{
	gpio_set_value(pdat->mosi_pin, state);
}

static inline int spi_gpio_getmiso(struct spi_gpio_pdata_t * pdat)
{
	return gpio_get_value(pdat->miso_pin);
}

/*
 * CPHA = 0
 * CPOL = 0
 */
static u32_t spi_gpio_bitbang_txrx_mode0(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(pdat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(pdat, 1);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(pdat);
		spi_gpio_setsclk(pdat, 0);
	}
	return val;
}

/*
 * CPHA = 1
 * CPOL = 0
 */
static u32_t spi_gpio_bitbang_txrx_mode1(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		spi_gpio_setsclk(pdat, 1);
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(pdat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(pdat, 0);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(pdat);
	}
	return val;
}

/*
 * CPHA = 0
 * CPOL = 1
 */
static u32_t spi_gpio_bitbang_txrx_mode2(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(pdat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(pdat, 0);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(pdat);
		spi_gpio_setsclk(pdat, 1);
	}
	return val;
}

/*
 * CPHA = 1
 * CPOL = 1
 */
static u32_t spi_gpio_bitbang_txrx_mode3(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		spi_gpio_setsclk(pdat, 0);
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(pdat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(pdat, 1);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(pdat);
	}
	return val;
}

static int spi_gpio_bitbang_xfer_8(struct spi_gpio_pdata_t * pdat,
	u32_t (*txrx)(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns),
	int ns, struct spi_msg_t * msg)
{
	int count = msg->len;
	int bits = msg->bits;
	u8_t * tx = msg->txbuf;
	u8_t * rx = msg->rxbuf;
	u8_t val;

	while(count > 0)
	{
		val = 0;
		if(tx)
			val = *tx++;
		val = txrx(pdat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 1;
	}
	return msg->len - count;
}

static int spi_gpio_bitbang_xfer_16(struct spi_gpio_pdata_t * pdat,
	u32_t (*txrx)(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns),
	int ns, struct spi_msg_t * msg)
{
	int count = msg->len;
	int bits = msg->bits;
	u16_t * tx = msg->txbuf;
	u16_t * rx = msg->rxbuf;
	u16_t val;

	while(count > 1)
	{
		val = 0;
		if(tx)
			val = *tx++;
		val = txrx(pdat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 2;
	}
	return msg->len - count;
}

static int spi_gpio_bitbang_xfer_32(struct spi_gpio_pdata_t * pdat,
	u32_t (*txrx)(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns),
	int ns, struct spi_msg_t * msg)
{
	int count = msg->len;
	int bits = msg->bits;
	u32_t * tx = msg->txbuf;
	u32_t * rx = msg->rxbuf;
	u32_t val;

	while(count > 3)
	{
		val = 0;
		if(tx)
			val = *tx++;
		val = txrx(pdat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 4;
	}
	return msg->len - count;
}

static int spi_gpio_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_gpio_pdata_t * pdat = (struct spi_gpio_pdata_t *)spi->priv;
	int (*xfer)(struct spi_gpio_pdata_t * pdat,
		u32_t (*txrx)(struct spi_gpio_pdata_t * pdat, u32_t val, int bits, int ns),
		int ns, struct spi_msg_t * msg);
	int ns;

	if(msg->bits <= 8)
		xfer = spi_gpio_bitbang_xfer_8;
	else if(msg->bits <= 16)
		xfer = spi_gpio_bitbang_xfer_16;
	else if(msg->bits <= 32)
		xfer = spi_gpio_bitbang_xfer_32;
	else
		return 0;

	if(msg->speed > 0)
		ns = (1000000000 / 2) / msg->speed;
	else
		ns = 10;

	switch(msg->mode & 0x3)
	{
	case 0:
		return xfer(pdat, spi_gpio_bitbang_txrx_mode0, ns, msg);
	case 1:
		return xfer(pdat, spi_gpio_bitbang_txrx_mode1, ns, msg);
	case 2:
		return xfer(pdat, spi_gpio_bitbang_txrx_mode2, ns, msg);
	case 3:
		return xfer(pdat, spi_gpio_bitbang_txrx_mode3, ns, msg);
	default:
		break;
	}
	return 0;
}

static void spi_gpio_chipselect(struct spi_t * spi, int enable)
{
	struct spi_gpio_pdata_t * pdat = (struct spi_gpio_pdata_t *)spi->priv;
	if(pdat->cs_pin >= 0)
		gpio_set_value(pdat->cs_pin, enable ? 0 : 1);
}

static struct device_t * spi_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_gpio_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;

	if(!gpio_is_valid(dt_read_int(n, "sclk-pin", -1)))
		return NULL;

	if(!gpio_is_valid(dt_read_int(n, "mosi-pin", -1)))
		return NULL;

	if(!gpio_is_valid(dt_read_int(n, "miso-pin", -1)))
		return NULL;

	if(!gpio_is_valid(dt_read_int(n, "cs-pin", -1)))
		return NULL;

	pdat = malloc(sizeof(struct spi_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(pdat);
		return FALSE;
	}

	pdat->sclk_pin = dt_read_int(n, "sclk-pin", -1);
	pdat->mosi_pin = dt_read_int(n, "mosi-pin", -1);
	pdat->miso_pin = dt_read_int(n, "miso-pin", -1);
	pdat->cs_pin = dt_read_int(n, "cs-pin", -1);

	if(pdat->sclk_pin)
		gpio_direction_output(pdat->sclk_pin, 0);
	if(pdat->mosi_pin)
		gpio_direction_output(pdat->mosi_pin, 0);
	if(pdat->miso_pin)
		gpio_direction_input(pdat->miso_pin);
	if(pdat->cs_pin)
		gpio_direction_output(pdat->cs_pin, 1);

	spi->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	spi->transfer = spi_gpio_transfer,
	spi->chipselect = spi_gpio_chipselect,
	spi->priv = pdat;

	if(!register_spi(&dev, spi))
	{
		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void spi_gpio_remove(struct device_t * dev)
{
	struct spi_t * spi = (struct spi_t *)dev->priv;

	if(spi && unregister_spi(spi))
	{
		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
	}
}

static void spi_gpio_suspend(struct device_t * dev)
{
}

static void spi_gpio_resume(struct device_t * dev)
{
}

struct driver_t spi_gpio = {
	.name		= "spi-gpio",
	.probe		= spi_gpio_probe,
	.remove		= spi_gpio_remove,
	.suspend	= spi_gpio_suspend,
	.resume		= spi_gpio_resume,
};

static __init void spi_gpio_driver_init(void)
{
	register_driver(&spi_gpio);
}

static __exit void spi_gpio_driver_exit(void)
{
	unregister_driver(&spi_gpio);
}

driver_initcall(spi_gpio_driver_init);
driver_exitcall(spi_gpio_driver_exit);
