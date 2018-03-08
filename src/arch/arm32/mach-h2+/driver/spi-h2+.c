/*
 * driver/spi-h3.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <spi/spi.h>

enum {
	SPI_GCR	= 0x04,
	SPI_TCR	= 0x08,
	SPI_IER	= 0x10,
	SPI_ISR	= 0x14,
	SPI_FCR	= 0x18,
	SPI_FSR	= 0x1c,
	SPI_WCR	= 0x20,
	SPI_CCR	= 0x24,
	SPI_MBC	= 0x30,
	SPI_MTC	= 0x34,
	SPI_BCC	= 0x38,
	SPI_TXD	= 0x200,
	SPI_RXD	= 0x300,
};

struct spi_h3_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int sclk;
	int sclkcfg;
	int mosi;
	int mosicfg;
	int miso;
	int misocfg;
	int cs;
	int cscfg;
};

static void h3_spi_enable_chip(struct spi_h3_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt + SPI_GCR);
	val |= (1 << 31) | (1 << 7) | (1 << 1) | (1 << 0);
	write32(pdat->virt + SPI_GCR, val);
	while(read32(pdat->virt + SPI_GCR) & (1 << 31));

	val = read32(pdat->virt + SPI_TCR);
	val |= (1 << 6) | (1 << 2);
	write32(pdat->virt + SPI_TCR, val);

	val = read32(pdat->virt + SPI_FCR);
	val |= (1 << 31) | (1 << 15);
	write32(pdat->virt + SPI_FCR, val);
}

static void h3_spi_set_rate(struct spi_h3_pdata_t * pdat, u64_t rate)
{
	u64_t pclk = clk_get_rate(pdat->clk);
	u32_t div, val;

	div = pclk / (2 * rate);
	if(div <= (0xff + 1))
	{
		if(div > 0)
			div--;
		val = ((div & 0xff) << 0) | (1 << 12);
	}
	else
	{
		div = ilog2(pclk) - ilog2(rate);
		val = ((div & 0xf) << 8);
	}
	write32(pdat->virt + SPI_CCR, val);
}

static void h3_spi_set_mode(struct spi_h3_pdata_t * pdat, int mode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~(0x3 << 0);
	val |= (mode & 0x3) << 0;
	write32(pdat->virt + SPI_TCR, val);
}

static void h3_spi_write_txbuf(struct spi_h3_pdata_t * pdat, u8_t * buf, int len)
{
	int i;

	if(!buf)
		len = 0;

	write32(pdat->virt + SPI_MTC, len & 0xffffff);
	write32(pdat->virt + SPI_BCC, len & 0xffffff);
	for(i = 0; i < len; ++i)
		write8(pdat->virt + SPI_TXD, *buf++);
}

static int h3_spi_xfer(struct spi_h3_pdata_t * pdat, struct spi_msg_t * msg)
{
	int count = msg->len * msg->bits / 8;
	u8_t * tx = msg->txbuf;
	u8_t * rx = msg->rxbuf;
	u8_t val;
	int n, i;

	while(count > 0)
	{
		n = (count <= 64) ? count : 64;
		write32(pdat->virt + SPI_MBC, n);
		h3_spi_write_txbuf(pdat, tx, n);
		write32(pdat->virt + SPI_TCR, read32(pdat->virt + SPI_TCR) | (1 << 31));

		while((read32(pdat->virt + SPI_FSR) & 0xff) < n);
		for(i = 0; i < n; i++)
		{
			val = read8(pdat->virt + SPI_RXD);
			if(rx)
				*rx++ = val;
		}

		if(tx)
			tx += n;
		count -= n;
	}
	return msg->len;
}

static int spi_h3_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_h3_pdata_t * pdat = (struct spi_h3_pdata_t *)spi->priv;

	h3_spi_set_mode(pdat, msg->mode);
	h3_spi_set_rate(pdat, (msg->speed > 0) ? msg->speed : 1000000);
	return h3_spi_xfer(pdat, msg);
}

static void spi_h3_select(struct spi_t * spi, int cs)
{
	struct spi_h3_pdata_t * pdat = (struct spi_h3_pdata_t *)spi->priv;
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((cs & 0x3) << 4) | (0x0 << 7);
	write32(pdat->virt + SPI_TCR, val);
}

static void spi_h3_deselect(struct spi_t * spi, int cs)
{
	struct spi_h3_pdata_t * pdat = (struct spi_h3_pdata_t *)spi->priv;
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((cs & 0x3) << 4) | (0x1 << 7);
	write32(pdat->virt + SPI_TCR, val);
}

static struct device_t * spi_h3_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_h3_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct spi_h3_pdata_t));
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
	pdat->mosi = dt_read_int(n, "mosi-gpio", -1);;
	pdat->mosicfg = dt_read_int(n, "mosi-gpio-config", -1);
	pdat->miso = dt_read_int(n, "miso-gpio", -1);;
	pdat->misocfg = dt_read_int(n, "miso-gpio-config", -1);
	pdat->cs = dt_read_int(n, "cs-gpio", -1);
	pdat->cscfg = dt_read_int(n, "cs-gpio-config", -1);

	spi->name = alloc_device_name(dt_read_name(n), -1);
	spi->transfer = spi_h3_transfer;
	spi->select = spi_h3_select;
	spi->deselect = spi_h3_deselect;
	spi->priv = pdat;

	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	if(pdat->sclk >= 0)
	{
		if(pdat->sclkcfg >= 0)
			gpio_set_cfg(pdat->sclk, pdat->sclkcfg);
		gpio_set_pull(pdat->sclk, GPIO_PULL_NONE);
	}
	if(pdat->mosi >= 0)
	{
		if(pdat->mosicfg >= 0)
			gpio_set_cfg(pdat->mosi, pdat->mosicfg);
		gpio_set_pull(pdat->mosi, GPIO_PULL_NONE);
	}
	if(pdat->miso >= 0)
	{
		if(pdat->misocfg >= 0)
			gpio_set_cfg(pdat->miso, pdat->misocfg);
		gpio_set_pull(pdat->miso, GPIO_PULL_NONE);
	}
	if(pdat->cs >= 0)
	{
		if(pdat->cscfg >= 0)
			gpio_set_cfg(pdat->cs, pdat->cscfg);
		gpio_set_pull(pdat->cs, GPIO_PULL_NONE);
	}
	h3_spi_enable_chip(pdat);

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

static void spi_h3_remove(struct device_t * dev)
{
	struct spi_t * spi = (struct spi_t *)dev->priv;
	struct spi_h3_pdata_t * pdat = (struct spi_h3_pdata_t *)spi->priv;

	if(spi && unregister_spi(spi))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
	}
}

static void spi_h3_suspend(struct device_t * dev)
{
}

static void spi_h3_resume(struct device_t * dev)
{
}

static struct driver_t spi_h3 = {
	.name		= "spi-h3",
	.probe		= spi_h3_probe,
	.remove		= spi_h3_remove,
	.suspend	= spi_h3_suspend,
	.resume		= spi_h3_resume,
};

static __init void spi_h3_driver_init(void)
{
	register_driver(&spi_h3);
}

static __exit void spi_h3_driver_exit(void)
{
	unregister_driver(&spi_h3);
}

driver_initcall(spi_h3_driver_init);
driver_exitcall(spi_h3_driver_exit);
