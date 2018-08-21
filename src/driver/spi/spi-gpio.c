/*
 * driver/spi/spi-gpio.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>
#include <spi/spi.h>

struct spi_gpio_pdata_t {
	int sclk;
	int sclkcfg;
	int mosi;
	int mosicfg;
	int miso;
	int misocfg;
	int cs;
	int cscfg;
};

static inline void spi_gpio_setsclk(struct spi_gpio_pdata_t * pdat, int state)
{
	gpio_set_value(pdat->sclk, state);
}

static inline void spi_gpio_setmosi(struct spi_gpio_pdata_t * pdat, int state)
{
	gpio_set_value(pdat->mosi, state);
}

static inline int spi_gpio_getmiso(struct spi_gpio_pdata_t * pdat)
{
	return gpio_get_value(pdat->miso);
}

/*
 * CPOL = 0, CPHA = 0
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
 * CPOL = 0, CPHA = 1
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
 * CPOL = 1, CPHA = 0
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
 * CPOL = 1, CPHA = 1
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

static void spi_gpio_select(struct spi_t * spi, int cs)
{
	struct spi_gpio_pdata_t * pdat = (struct spi_gpio_pdata_t *)spi->priv;
	if(pdat->cs >= 0)
		gpio_set_value(pdat->cs, 0);
}

static void spi_gpio_deselect(struct spi_t * spi, int cs)
{
	struct spi_gpio_pdata_t * pdat = (struct spi_gpio_pdata_t *)spi->priv;
	if(pdat->cs >= 0)
		gpio_set_value(pdat->cs, 1);
}

static struct device_t * spi_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_gpio_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;
	int sclk = dt_read_int(n, "sclk-gpio", -1);
	int mosi = dt_read_int(n, "mosi-gpio", -1);
	int miso = dt_read_int(n, "miso-gpio", -1);

	if(!gpio_is_valid(sclk) || !gpio_is_valid(mosi) || !gpio_is_valid(miso))
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

	pdat->sclk = sclk;
	pdat->sclkcfg = dt_read_int(n, "sclk-gpio-config", -1);
	pdat->mosi = mosi;
	pdat->mosicfg = dt_read_int(n, "mosi-gpio-config", -1);
	pdat->miso = miso;
	pdat->misocfg = dt_read_int(n, "miso-gpio-config", -1);
	pdat->cs = dt_read_int(n, "cs-gpio", -1);
	pdat->cscfg = dt_read_int(n, "cs-gpio-config", -1);

	if(pdat->sclk >= 0)
	{
		if(pdat->sclkcfg >= 0)
			gpio_set_cfg(pdat->sclk, pdat->sclkcfg);
		gpio_set_pull(pdat->sclk, GPIO_PULL_UP);
		gpio_direction_output(pdat->sclk, 0);
	}
	if(pdat->mosi >= 0)
	{
		if(pdat->mosicfg >= 0)
			gpio_set_cfg(pdat->mosi, pdat->mosicfg);
		gpio_set_pull(pdat->mosi, GPIO_PULL_UP);
		gpio_direction_output(pdat->mosi, 0);
	}
	if(pdat->miso >= 0)
	{
		if(pdat->misocfg >= 0)
			gpio_set_cfg(pdat->miso, pdat->misocfg);
		gpio_set_pull(pdat->miso, GPIO_PULL_UP);
		gpio_direction_input(pdat->miso);
	}
	if(pdat->cs >= 0)
	{
		if(pdat->cscfg >= 0)
			gpio_set_cfg(pdat->cs, pdat->cscfg);
		gpio_set_pull(pdat->cs, GPIO_PULL_UP);
		gpio_direction_output(pdat->cs, 1);
	}

	spi->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	spi->transfer = spi_gpio_transfer;
	spi->select = spi_gpio_select;
	spi->deselect = spi_gpio_deselect;
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

static struct driver_t spi_gpio = {
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
