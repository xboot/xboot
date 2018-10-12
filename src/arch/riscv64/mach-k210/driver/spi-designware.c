/*
 * driver/spi-designware.c
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <spi/spi.h>

enum {
	SPI_CTRL0	= 0x00,
	SPI_CTRL1	= 0x04,
	SPI_SSIENR	= 0x08,
	SPI_MWCR	= 0x0c,
	SPI_SER		= 0x10,
	SPI_BAUDR	= 0x14,
	SPI_TXFLTR	= 0x18,
	SPI_RXFLTR	= 0x1c,
	SPI_TXFLR	= 0x20,
	SPI_RXFLR	= 0x24,
	SPI_SR		= 0x28,
	SPI_IMR		= 0x2c,
	SPI_ISR		= 0x30,
	SPI_RISR	= 0x34,
	SPI_TXOICR	= 0x38,
	SPI_RXOICR	= 0x3c,
	SPI_RXUICR	= 0x40,
	SPI_MSTICR	= 0x44,
	SPI_ICR		= 0x48,
	SPI_DMACR	= 0x4c,
	SPI_DMATDLR	= 0x50,
	SPI_DMARDLR	= 0x54,
	SPI_IDR		= 0x58,
	SPI_VERSION	= 0x5c,
	SPI_DR		= 0x60,
};

struct spi_designware_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int sclk;
	int sclkcfg;
	int io0;
	int io0cfg;
	int io1;
	int io1cfg;
	int io2;
	int io2cfg;
	int io3;
	int io3cfg;
	int io4;
	int io4cfg;
	int io5;
	int io5cfg;
	int io6;
	int io6cfg;
	int io7;
	int io7cfg;
	int cs0;
	int cs0cfg;
	int cs1;
	int cs1cfg;
	int cs2;
	int cs2cfg;
	int cs3;
	int cs3cfg;
	int fifo_len;
	int type_offset;
	int mode_offset;
	int bits_offset;
	int tmode_offset;
};

static inline void spi_designware_enable_chip(struct spi_designware_pdata_t * pdat, int enable)
{
	write32(pdat->virt + SPI_SSIENR, enable ? 1 : 0);
}

static inline void spi_designware_set_rate(struct spi_designware_pdata_t * pdat, u64_t rate)
{
	u32_t div;

	spi_designware_enable_chip(pdat, 0);
	div = clk_get_rate(pdat->clk) / rate;
	div = (div + 1) & 0xfffe;
	write32(pdat->virt + SPI_BAUDR, div);
	spi_designware_enable_chip(pdat, 1);
}

static inline void spi_designware_set_type(struct spi_designware_pdata_t * pdat, int type)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << pdat->type_offset);
	switch(type)
	{
	case SPI_TYPE_SINGLE:
		val |= 0x0 << pdat->type_offset;
		break;
	case SPI_TYPE_DUAL:
		val |= 0x1 << pdat->type_offset;
		break;
	case SPI_TYPE_QUAD:
		val |= 0x2 << pdat->type_offset;
		break;
	case SPI_TYPE_OCTAL:
		val |= 0x3 << pdat->type_offset;
		break;
	default:
		val |= 0x0 << pdat->type_offset;
		break;
	}
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_designware_set_mode(struct spi_designware_pdata_t * pdat, int mode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << pdat->mode_offset);
	val |= (mode & 0x3) << pdat->mode_offset;
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_designware_set_bits(struct spi_designware_pdata_t * pdat, int bits)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0xf << pdat->bits_offset);
	val |= ((bits - 1) & 0xf) << pdat->bits_offset;
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_designware_set_tmode(struct spi_designware_pdata_t * pdat, int tmode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << pdat->tmode_offset);
	val |= (tmode & 0x3) << pdat->tmode_offset;
	write32(pdat->virt + SPI_CTRL0, val);
}

static int spi_designware_write_txbuf_8(struct spi_designware_pdata_t * pdat, u8_t * buf, int cnt)
{
	int txleft = pdat->fifo_len - read32(pdat->virt + SPI_TXFLR);
	int rxleft = pdat->fifo_len - read32(pdat->virt + SPI_RXFLR);
	int left = (txleft < rxleft) ? txleft : rxleft;
	int len = (cnt < left) ? cnt : left;
	int i;

	for(i = 0; i < len; i++)
	{
		LOG("W=%02x, %d, %d, %d", *buf, left, len, cnt);
		write8(pdat->virt + SPI_DR, *buf++);
	}
	return len;
}

static int spi_designware_read_rxbuf_8(struct spi_designware_pdata_t * pdat, u8_t * buf, int cnt)
{
	int left = read32(pdat->virt + SPI_RXFLR);
	int len = (cnt < left) ? cnt : left;
	int i;
	u8_t v;
	for(i = 0; i < len; i++)
	{
		v = read8(pdat->virt + SPI_DR);
		*buf++ = v;
		LOG("R=%02x, %d, %d, %d", v, left, len, cnt);
	}
	return len;
}

static int spi_designware_xfer_8(struct spi_designware_pdata_t * pdat, struct spi_msg_t * msg)
{
	u8_t * tx = msg->txbuf;
	u8_t * rx = msg->rxbuf;
	int txlen = msg->len;
	int rxlen = msg->len;
	int n;

	while((txlen > 0) || (rxlen > 0))
	{
		n = spi_designware_write_txbuf_8(pdat, tx, txlen);
		txlen -= n;
		if(tx)
			tx += n;

		n = spi_designware_read_rxbuf_8(pdat, rx, rxlen);
		rxlen -= n;
		if(rx)
			rx += n;
	}
	return msg->len - txlen;
}

static int spi_designware_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_designware_pdata_t * pdat = (struct spi_designware_pdata_t *)spi->priv;
	int r;

	spi_designware_set_type(pdat, msg->type);
	spi_designware_set_mode(pdat, msg->mode);
	spi_designware_set_tmode(pdat, 0);
	spi_designware_set_bits(pdat, msg->bits);
	spi_designware_set_rate(pdat, (msg->speed > 0) ? msg->speed : 1000000);

//	spi_designware_enable_chip(pdat, 1);
	write32(pdat->virt + SPI_SER, 0x1 << 0);

	r = spi_designware_xfer_8(pdat, msg);

	write32(pdat->virt + SPI_SER, 0);
//	spi_designware_enable_chip(pdat, 0);

	return r;
}

static void spi_designware_select(struct spi_t * spi, int cs)
{
	struct spi_designware_pdata_t * pdat = (struct spi_designware_pdata_t *)spi->priv;
	write32(pdat->virt + SPI_SER, 0x1 << cs);
}

static void spi_designware_deselect(struct spi_t * spi, int cs)
{
	struct spi_designware_pdata_t * pdat = (struct spi_designware_pdata_t *)spi->priv;
	write32(pdat->virt + SPI_SER, 0x0);
}

static struct device_t * spi_designware_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_designware_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct spi_designware_pdata_t));
	if(!pdat)
		return FALSE;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(pdat);
		return FALSE;
	}

	clk_enable(clk);
	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->sclk = dt_read_int(n, "sclk-gpio", -1);
	pdat->sclkcfg = dt_read_int(n, "sclk-gpio-config", -1);
	pdat->io0 = dt_read_int(n, "io0-gpio", -1);
	pdat->io0cfg = dt_read_int(n, "io0-gpio-config", -1);
	pdat->io1 = dt_read_int(n, "io1-gpio", -1);
	pdat->io1cfg = dt_read_int(n, "io1-gpio-config", -1);
	pdat->io2 = dt_read_int(n, "io2-gpio", -1);
	pdat->io2cfg = dt_read_int(n, "io2-gpio-config", -1);
	pdat->io3 = dt_read_int(n, "io3-gpio", -1);
	pdat->io3cfg = dt_read_int(n, "io3-gpio-config", -1);
	pdat->io4 = dt_read_int(n, "io4-gpio", -1);
	pdat->io4cfg = dt_read_int(n, "io4-gpio-config", -1);
	pdat->io5 = dt_read_int(n, "io5-gpio", -1);
	pdat->io5cfg = dt_read_int(n, "io5-gpio-config", -1);
	pdat->io6 = dt_read_int(n, "io6-gpio", -1);
	pdat->io6cfg = dt_read_int(n, "io6-gpio-config", -1);
	pdat->io7 = dt_read_int(n, "io7-gpio", -1);
	pdat->io7cfg = dt_read_int(n, "io7-gpio-config", -1);
	pdat->cs0 = dt_read_int(n, "cs0-gpio", -1);
	pdat->cs0cfg = dt_read_int(n, "cs0-gpio-config", -1);
	pdat->cs1 = dt_read_int(n, "cs1-gpio", -1);
	pdat->cs1cfg = dt_read_int(n, "cs1-gpio-config", -1);
	pdat->cs2 = dt_read_int(n, "cs2-gpio", -1);
	pdat->cs2cfg = dt_read_int(n, "cs2-gpio-config", -1);
	pdat->cs3 = dt_read_int(n, "cs3-gpio", -1);
	pdat->cs3cfg = dt_read_int(n, "cs3-gpio-config", -1);
	pdat->fifo_len = dt_read_int(n, "fifo-length", 32);
	pdat->type_offset = dt_read_int(n, "type-offset", 22);
	pdat->mode_offset = dt_read_int(n, "mode-offset", 8);
	pdat->bits_offset = dt_read_int(n, "bits-offset", 0);
	pdat->tmode_offset = dt_read_int(n, "tmode-offset", 10);

	spi->name = alloc_device_name(dt_read_name(n), -1);
	spi->type = SPI_TYPE_SINGLE;
	spi->transfer = spi_designware_transfer;
	spi->select = spi_designware_select;
	spi->deselect = spi_designware_deselect;
	spi->priv = pdat;

	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	if(pdat->sclk >= 0)
	{
		if(pdat->sclkcfg >= 0)
			gpio_set_cfg(pdat->sclk, pdat->sclkcfg);
		gpio_set_pull(pdat->sclk, GPIO_PULL_NONE);
	}
	if(pdat->io0 >= 0)
	{
		if(pdat->io0cfg >= 0)
			gpio_set_cfg(pdat->io0, pdat->io0cfg);
		gpio_set_pull(pdat->io0, GPIO_PULL_NONE);
	}
	if(pdat->io1 >= 0)
	{
		if(pdat->io1cfg >= 0)
			gpio_set_cfg(pdat->io1, pdat->io1cfg);
		gpio_set_pull(pdat->io1, GPIO_PULL_NONE);
	}
	if(pdat->io2 >= 0)
	{
		if(pdat->io2cfg >= 0)
			gpio_set_cfg(pdat->io2, pdat->io2cfg);
		gpio_set_pull(pdat->io2, GPIO_PULL_NONE);
	}
	if(pdat->io3 >= 0)
	{
		if(pdat->io3cfg >= 0)
			gpio_set_cfg(pdat->io3, pdat->io3cfg);
		gpio_set_pull(pdat->io3, GPIO_PULL_NONE);
	}
	if(pdat->io4 >= 0)
	{
		if(pdat->io4cfg >= 0)
			gpio_set_cfg(pdat->io4, pdat->io4cfg);
		gpio_set_pull(pdat->io4, GPIO_PULL_NONE);
	}
	if(pdat->io5 >= 0)
	{
		if(pdat->io5cfg >= 0)
			gpio_set_cfg(pdat->io5, pdat->io5cfg);
		gpio_set_pull(pdat->io5, GPIO_PULL_NONE);
	}
	if(pdat->io6 >= 0)
	{
		if(pdat->io6cfg >= 0)
			gpio_set_cfg(pdat->io6, pdat->io6cfg);
		gpio_set_pull(pdat->io6, GPIO_PULL_NONE);
	}
	if(pdat->io7 >= 0)
	{
		if(pdat->io7cfg >= 0)
			gpio_set_cfg(pdat->io7, pdat->io7cfg);
		gpio_set_pull(pdat->io7, GPIO_PULL_NONE);
	}
	if(pdat->cs0 >= 0)
	{
		if(pdat->cs0cfg >= 0)
			gpio_set_cfg(pdat->cs0, pdat->cs0cfg);
		gpio_set_pull(pdat->cs0, GPIO_PULL_NONE);
	}
	if(pdat->cs1 >= 0)
	{
		if(pdat->cs1cfg >= 0)
			gpio_set_cfg(pdat->cs1, pdat->cs1cfg);
		gpio_set_pull(pdat->cs1, GPIO_PULL_NONE);
	}
	if(pdat->cs2 >= 0)
	{
		if(pdat->cs2cfg >= 0)
			gpio_set_cfg(pdat->cs2, pdat->cs2cfg);
		gpio_set_pull(pdat->cs2, GPIO_PULL_NONE);
	}
	if(pdat->cs3 >= 0)
	{
		if(pdat->cs3cfg >= 0)
			gpio_set_cfg(pdat->cs3, pdat->cs3cfg);
		gpio_set_pull(pdat->cs3, GPIO_PULL_NONE);
	}
	write32(pdat->virt + SPI_IMR, 0xff);
	write32(pdat->virt + SPI_DMACR, 0);
	write32(pdat->virt + SPI_DMATDLR, 0);
	write32(pdat->virt + SPI_DMARDLR, 0);
	write32(pdat->virt + SPI_SER, 0);
	write32(pdat->virt + SPI_SSIENR, 0);
	write32(pdat->virt + SPI_CTRL0, 0);
	spi_designware_enable_chip(pdat, 1);

	if(!register_spi(&dev, spi))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void spi_designware_remove(struct device_t * dev)
{
	struct spi_t * spi = (struct spi_t *)dev->priv;
	struct spi_designware_pdata_t * pdat = (struct spi_designware_pdata_t *)spi->priv;

	if(spi && unregister_spi(spi))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
	}
}

static void spi_designware_suspend(struct device_t * dev)
{
}

static void spi_designware_resume(struct device_t * dev)
{
}

static struct driver_t spi_designware = {
	.name		= "spi-designware",
	.probe		= spi_designware_probe,
	.remove		= spi_designware_remove,
	.suspend	= spi_designware_suspend,
	.resume		= spi_designware_resume,
};

static __init void spi_designware_driver_init(void)
{
	register_driver(&spi_designware);
}

static __exit void spi_designware_driver_exit(void)
{
	unregister_driver(&spi_designware);
}

driver_initcall(spi_designware_driver_init);
driver_exitcall(spi_designware_driver_exit);
