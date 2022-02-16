/*
 * driver/spi-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

struct spi_t113_pdata_t {
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

static void t113_spi_enable_chip(struct spi_t113_pdata_t * pdat)
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

static void t113_spi_set_rate(struct spi_t113_pdata_t * pdat, u64_t rate)
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

static void t113_spi_set_mode(struct spi_t113_pdata_t * pdat, int mode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~(0x3 << 0);
	val |= (mode & 0x3) << 0;
	write32(pdat->virt + SPI_TCR, val);
}

static void t113_spi_write_txbuf(struct spi_t113_pdata_t * pdat, u8_t * buf, int len)
{
	int i;

	write32(pdat->virt + SPI_MTC, len & 0xffffff);
	write32(pdat->virt + SPI_BCC, len & 0xffffff);
	if(buf)
	{
		for(i = 0; i < len; i++)
			write8(pdat->virt + SPI_TXD, *buf++);
	}
	else
	{
		for(i = 0; i < len; i++)
			write8(pdat->virt + SPI_TXD, 0xff);
	}
}

static int t113_spi_xfer(struct spi_t113_pdata_t * pdat, struct spi_msg_t * msg)
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
		t113_spi_write_txbuf(pdat, tx, n);
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

static int spi_t113_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_t113_pdata_t * pdat = (struct spi_t113_pdata_t *)spi->priv;

	t113_spi_set_mode(pdat, msg->mode);
	t113_spi_set_rate(pdat, (msg->speed > 0) ? msg->speed : 1000000);
	return t113_spi_xfer(pdat, msg);
}

static void spi_t113_select(struct spi_t * spi, int cs)
{
	struct spi_t113_pdata_t * pdat = (struct spi_t113_pdata_t *)spi->priv;
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((cs & 0x3) << 4) | (0x0 << 7);
	write32(pdat->virt + SPI_TCR, val);
}

static void spi_t113_deselect(struct spi_t * spi, int cs)
{
	struct spi_t113_pdata_t * pdat = (struct spi_t113_pdata_t *)spi->priv;
	u32_t val;

	val = read32(pdat->virt + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((cs & 0x3) << 4) | (0x1 << 7);
	write32(pdat->virt + SPI_TCR, val);
}

static struct device_t * spi_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_t113_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct spi_t113_pdata_t));
	if(!pdat)
		return NULL;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(pdat);
		return NULL;
	}

	clk_enable(clk);
	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->sclk = dt_read_int(n, "sclk-gpio", -1);
	pdat->sclkcfg = dt_read_int(n, "sclk-gpio-config", -1);
	pdat->mosi = dt_read_int(n, "mosi-gpio", -1);
	pdat->mosicfg = dt_read_int(n, "mosi-gpio-config", -1);
	pdat->miso = dt_read_int(n, "miso-gpio", -1);
	pdat->misocfg = dt_read_int(n, "miso-gpio-config", -1);
	pdat->cs = dt_read_int(n, "cs-gpio", -1);
	pdat->cscfg = dt_read_int(n, "cs-gpio-config", -1);

	spi->name = alloc_device_name(dt_read_name(n), -1);
	spi->type = SPI_TYPE_SINGLE;
	spi->transfer = spi_t113_transfer;
	spi->select = spi_t113_select;
	spi->deselect = spi_t113_deselect;
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
	t113_spi_enable_chip(pdat);

	if(!(dev = register_spi(spi, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
		return NULL;
	}
	return dev;
}

static void spi_t113_remove(struct device_t * dev)
{
	struct spi_t * spi = (struct spi_t *)dev->priv;
	struct spi_t113_pdata_t * pdat = (struct spi_t113_pdata_t *)spi->priv;

	if(spi)
	{
		unregister_spi(spi);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
	}
}

static void spi_t113_suspend(struct device_t * dev)
{
}

static void spi_t113_resume(struct device_t * dev)
{
}

static struct driver_t spi_t113 = {
	.name		= "spi-t113",
	.probe		= spi_t113_probe,
	.remove		= spi_t113_remove,
	.suspend	= spi_t113_suspend,
	.resume		= spi_t113_resume,
};

static __init void spi_t113_driver_init(void)
{
	register_driver(&spi_t113);
}

static __exit void spi_t113_driver_exit(void)
{
	unregister_driver(&spi_t113);
}

driver_initcall(spi_t113_driver_init);
driver_exitcall(spi_t113_driver_exit);
