/*
 * drivers/bus/spi/spi-gpio.c
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
#include <bus/spi-gpio.h>

struct spi_gpio_private_data_t {
	int sclk_pin;
	int mosi_pin;
	int miso_pin;
	int cs_pin;
};

static inline void spi_gpio_setsclk(struct spi_gpio_private_data_t * dat, int state)
{
	gpio_set_value(dat->sclk_pin, state);
}

static inline void spi_gpio_setmosi(struct spi_gpio_private_data_t * dat, int state)
{
	gpio_set_value(dat->mosi_pin, state);
}

static inline int spi_gpio_getmiso(struct spi_gpio_private_data_t * dat)
{
	return gpio_get_value(dat->miso_pin);
}

/* CPHA = 0, CPOL = 0 */
static u32_t spi_gpio_bitbang_txrx_mode0(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(dat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(dat, 1);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(dat);
		spi_gpio_setsclk(dat, 0);
	}
	return val;
}

/* CPHA = 1, CPOL = 0 */
static u32_t spi_gpio_bitbang_txrx_mode1(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		spi_gpio_setsclk(dat, 1);
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(dat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(dat, 0);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(dat);
	}
	return val;
}

/* CPHA = 0, CPOL = 1 */
static u32_t spi_gpio_bitbang_txrx_mode2(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(dat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(dat, 0);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(dat);
		spi_gpio_setsclk(dat, 1);
	}
	return val;
}

/* CPHA = 1, CPOL = 1 */
static u32_t spi_gpio_bitbang_txrx_mode3(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns)
{
	u32_t oldbit = (!(val & (1 << (bits - 1)))) << 31;

	for(val <<= (32 - bits); bits > 0; bits--)
	{
		spi_gpio_setsclk(dat, 0);
		if((val & (1 << 31)) != oldbit)
		{
			spi_gpio_setmosi(dat, val & (1 << 31));
			oldbit = val & (1 << 31);
		}
		ndelay(ns);
		spi_gpio_setsclk(dat, 1);
		ndelay(ns);
		val <<= 1;
		val |= spi_gpio_getmiso(dat);
	}
	return val;
}

static int spi_gpio_bitbang_xfer_8(struct spi_gpio_private_data_t * dat,
	u32_t (*txrx)(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns),
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
		val = txrx(dat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 1;
	}
	return msg->len - count;
}

static int spi_gpio_bitbang_xfer_16(struct spi_gpio_private_data_t * dat,
	u32_t (*txrx)(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns),
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
		val = txrx(dat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 2;
	}
	return msg->len - count;
}

static int spi_gpio_bitbang_xfer_32(struct spi_gpio_private_data_t * dat,
	u32_t (*txrx)(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns),
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
		val = txrx(dat, val, bits, ns);
		if(rx)
			*rx++ = val;
		count -= 4;
	}
	return msg->len - count;
}

static void spi_gpio_init(struct spi_t * spi)
{
}

static void spi_gpio_exit(struct spi_t * spi)
{
}

static int spi_gpio_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_gpio_private_data_t * dat = (struct spi_gpio_private_data_t *)spi->priv;
	int (*xfer)(struct spi_gpio_private_data_t * dat,
		u32_t (*txrx)(struct spi_gpio_private_data_t * dat, u32_t val, int bits, int ns),
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
		return xfer(dat, spi_gpio_bitbang_txrx_mode0, ns, msg);
	case 1:
		return xfer(dat, spi_gpio_bitbang_txrx_mode1, ns, msg);
	case 2:
		return xfer(dat, spi_gpio_bitbang_txrx_mode2, ns, msg);
	case 3:
		return xfer(dat, spi_gpio_bitbang_txrx_mode3, ns, msg);
	default:
		break;
	}
	return 0;
}

static void spi_gpio_chipselect(struct spi_t * spi, int state)
{
	struct spi_gpio_private_data_t * dat = (struct spi_gpio_private_data_t *)spi->priv;
	if(dat->cs_pin >= 0)
		gpio_set_value(dat->cs_pin, state);
}

static bool_t spi_gpio_register_bus(struct resource_t * res)
{
	struct spi_gpio_data_t * rdat = (struct spi_gpio_data_t *)res->data;
	struct spi_gpio_private_data_t * dat;
	struct spi_t * spi;
	char name[64];

	dat = malloc(sizeof(struct spi_gpio_private_data_t));
	if(!dat)
		return FALSE;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->sclk_pin = rdat->sclk_pin;
	dat->mosi_pin = rdat->mosi_pin;
	dat->miso_pin = rdat->miso_pin;
	dat->cs_pin = rdat->cs_pin;

	if(dat->sclk_pin)
		gpio_direction_output(rdat->sclk_pin, 0);
	if(dat->mosi_pin)
		gpio_direction_output(rdat->mosi_pin, 0);
	if(dat->miso_pin)
		gpio_direction_input(rdat->miso_pin);
	if(dat->cs_pin)
		gpio_direction_output(rdat->cs_pin, 1);

	spi->name = strdup(name);
	spi->init = spi_gpio_init;
	spi->exit = spi_gpio_exit;
	spi->transfer = spi_gpio_transfer,
	spi->chipselect = spi_gpio_chipselect,
	spi->priv = dat;

	if(register_bus_spi(spi))
		return TRUE;

	free(spi->priv);
	free(spi->name);
	free(spi);
	return FALSE;
}

static bool_t spi_gpio_unregister_bus(struct resource_t * res)
{
	struct spi_t * spi;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	spi = search_bus_spi(name);
	if(!spi)
		return FALSE;

	if(!unregister_bus_spi(spi))
		return FALSE;

	free(spi->priv);
	free(spi->name);
	free(spi);
	return TRUE;
}

static __init void spi_gpio_bus_init(void)
{
	resource_for_each_with_name("spi-gpio", spi_gpio_register_bus);
}

static __exit void spi_gpio_bus_exit(void)
{
	resource_for_each_with_name("spi-gpio", spi_gpio_unregister_bus);
}

bus_initcall(spi_gpio_bus_init);
bus_exitcall(spi_gpio_bus_exit);
