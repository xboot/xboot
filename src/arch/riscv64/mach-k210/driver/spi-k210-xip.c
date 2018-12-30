/*
 * driver/spi-k210-xip.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
	SPI_CTRL0			= 0x000,
	SPI_CTRL1			= 0x004,
	SPI_SSIENR			= 0x008,
	SPI_MWCR			= 0x00c,
	SPI_SER				= 0x010,
	SPI_BAUDR			= 0x014,
	SPI_TXFTLR			= 0x018,
	SPI_RXFTLR			= 0x01c,
	SPI_TXFLR			= 0x020,
	SPI_RXFLR			= 0x024,
	SPI_SR				= 0x028,
	SPI_IMR				= 0x02c,
	SPI_ISR				= 0x030,
	SPI_RISR			= 0x034,
	SPI_TXOICR			= 0x038,
	SPI_RXOICR			= 0x03c,
	SPI_RXUICR			= 0x040,
	SPI_MSTICR			= 0x044,
	SPI_ICR				= 0x048,
	SPI_DMACR			= 0x04c,
	SPI_DMATDLR			= 0x050,
	SPI_DMARDLR			= 0x054,
	SPI_IDR				= 0x058,
	SPI_VERSION			= 0x05c,
	SPI_DR				= 0x060,
	SPI_RX_SAMPLE_DELAY	= 0x0f0,
	SPI_OTHER_CTRL0		= 0x0f4,
	SPI_XIP_MODE_BITS	= 0x0fc,
	SPI_XIP_INCR_INST	= 0x100,
	SPI_XIP_WRAP_INST	= 0x104,
	SPI_XIP_CTRL		= 0x108,
	SPI_XIP_SER			= 0x10c,
	SPI_XIP_RXOICR		= 0x110,
	SPI_XIP_TIMEOUT		= 0x114,
	SPI_ENDIAN			= 0x118,
};

struct spi_k210_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	int fifo_len;
};

static inline void spi_k210_enable_chip(struct spi_k210_pdata_t * pdat, int enable)
{
	write32(pdat->virt + SPI_SSIENR, enable ? 1 : 0);
}

static inline void spi_k210_set_rate(struct spi_k210_pdata_t * pdat, u64_t rate)
{
	u32_t div;

	div = clk_get_rate(pdat->clk) / rate;
	div = (div + 1) & 0xfffe;
	write32(pdat->virt + SPI_BAUDR, div);
}

static inline void spi_k210_set_type(struct spi_k210_pdata_t * pdat, int type)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << 22);
	switch(type)
	{
	case SPI_TYPE_SINGLE:
		val |= 0x0 << 22;
		break;
	case SPI_TYPE_DUAL:
		val |= 0x1 << 22;
		break;
	case SPI_TYPE_QUAD:
		val |= 0x2 << 22;
		break;
	case SPI_TYPE_OCTAL:
		val |= 0x3 << 22;
		break;
	default:
		val |= 0x0 << 22;
		break;
	}
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_k210_set_mode(struct spi_k210_pdata_t * pdat, int mode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << 8);
	val |= (mode & 0x3) << 8;
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_k210_set_bits(struct spi_k210_pdata_t * pdat, int bits)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0xf << 0);
	val |= ((bits - 1) & 0xf) << 0;
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_k210_set_tmode(struct spi_k210_pdata_t * pdat, int tmode)
{
	u32_t val;

	val = read32(pdat->virt + SPI_CTRL0);
	val &= ~(0x3 << 10);
	val |= (tmode & 0x3) << 10;
	write32(pdat->virt + SPI_CTRL0, val);
}

static inline void spi_k210_init(struct spi_k210_pdata_t * pdat)
{
	int i;

	write32(pdat->virt + SPI_IMR, 0x0);
	write32(pdat->virt + SPI_DMACR, 0x0);
	write32(pdat->virt + SPI_DMATDLR, 0x0);
	write32(pdat->virt + SPI_DMARDLR, 0x0);
	write32(pdat->virt + SPI_SER, 0x0);
	write32(pdat->virt + SPI_SSIENR, 0x0);
	write32(pdat->virt + SPI_CTRL0, (0 << 8) | (0 << 22) | ((8 - 1) << 0));
	write32(pdat->virt + SPI_OTHER_CTRL0, 0x0);

	spi_k210_set_rate(pdat, 1000000);
	spi_k210_set_type(pdat, SPI_TYPE_SINGLE);
	spi_k210_set_mode(pdat, 0);
	spi_k210_set_bits(pdat, 8);
	spi_k210_set_tmode(pdat, 3);

	spi_k210_enable_chip(pdat, 1);
	if(pdat->fifo_len <= 0)
	{
		for(i = 1; i < 256; i++)
		{
			write32(pdat->virt + SPI_TXFTLR, i);
			if(i != read32(pdat->virt + SPI_TXFTLR))
				break;
		}
		pdat->fifo_len = (i == 1) ? 0 : i;
	}
	spi_k210_enable_chip(pdat, 0);
}

static inline int spi_k210_xfer_8(struct spi_k210_pdata_t * pdat, struct spi_msg_t * msg)
{
	u8_t * tx = msg->txbuf;
	u8_t * rx = msg->rxbuf;
	int txlen = msg->len;
	int rxlen = msg->len;
	int i, n, t;

	if(tx && rx)
	{
		spi_k210_set_tmode(pdat, 0);
		while((txlen > 0) || (rxlen > 0))
		{
			if(txlen > 0)
			{
				n = pdat->fifo_len - read32(pdat->virt + SPI_TXFLR);
				t = pdat->fifo_len - read32(pdat->virt + SPI_RXFLR);
				n = n < t ? n : t;
				n = (n < txlen) ? n : txlen;
				for(i = 0; i < n; i++)
					write8(pdat->virt + SPI_DR, *tx++);
				txlen -= n;
			}
			if(rxlen > 0)
			{
				n = read32(pdat->virt + SPI_RXFLR);
				n = n < rxlen ? n : rxlen;
				for(i = 0; i < n; i++)
					*rx++ = (u8_t)read8(pdat->virt + SPI_DR);
				rxlen -= n;
			}
		}
	}
	else if(tx)
	{
		spi_k210_set_tmode(pdat, 1);
		while(txlen > 0)
		{
			n = pdat->fifo_len - read32(pdat->virt + SPI_TXFLR);
			n = (n < txlen) ? n : txlen;
			for(i = 0; i < n; i++)
				write8(pdat->virt + SPI_DR, *tx++);
			txlen -= n;
		}
	}
	else if(rx)
	{
		spi_k210_set_tmode(pdat, 2);
		write32(pdat->virt + SPI_CTRL1, (rxlen > 1) ? (rxlen - 1) : 0);
		while(rxlen > 0)
		{
			n = read32(pdat->virt + SPI_RXFLR);
			n = n < rxlen ? n : rxlen;
			for(i = 0; i < n; i++)
				*rx++ = (u8_t)read8(pdat->virt + SPI_DR);
			rxlen -= n;
		}
	}

	return msg->len;
}

static int spi_k210_transfer(struct spi_t * spi, struct spi_msg_t * msg)
{
	struct spi_k210_pdata_t * pdat = (struct spi_k210_pdata_t *)spi->priv;
	int ret = 0;

	spi_k210_set_type(pdat, msg->type);
	spi_k210_set_mode(pdat, msg->mode);
	spi_k210_set_bits(pdat, msg->bits);
	spi_k210_set_rate(pdat, (msg->speed > 0) ? msg->speed : 1000000);

	if(msg->bits <= 8)
	{
		ret = spi_k210_xfer_8(pdat, msg);
	}
	else if(msg->bits <= 16)
	{
	}
	return ret;
}

static void spi_k210_select(struct spi_t * spi, int cs)
{
	struct spi_k210_pdata_t * pdat = (struct spi_k210_pdata_t *)spi->priv;
	write32(pdat->virt + SPI_SER, read32(pdat->virt + SPI_SER) | (0x1 << cs));
	write32(pdat->virt + SPI_SSIENR, 0x1);
}

static void spi_k210_deselect(struct spi_t * spi, int cs)
{
	struct spi_k210_pdata_t * pdat = (struct spi_k210_pdata_t *)spi->priv;
	write32(pdat->virt + SPI_SSIENR, 0x0);
	write32(pdat->virt + SPI_SER, read32(pdat->virt + SPI_SER) & ~(0x1 << cs));
}

static struct device_t * spi_k210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_k210_pdata_t * pdat;
	struct spi_t * spi;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	pdat = malloc(sizeof(struct spi_k210_pdata_t));
	if(!pdat)
		return FALSE;

	spi = malloc(sizeof(struct spi_t));
	if(!spi)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->fifo_len = 0;

	spi->name = alloc_device_name(dt_read_name(n), -1);
	spi->type = SPI_TYPE_SINGLE | SPI_TYPE_DUAL | SPI_TYPE_QUAD | SPI_TYPE_OCTAL;
	spi->transfer = spi_k210_transfer;
	spi->select = spi_k210_select;
	spi->deselect = spi_k210_deselect;
	spi->priv = pdat;

	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	clk_enable(pdat->clk);
	spi_k210_init(pdat);

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

static void spi_k210_remove(struct device_t * dev)
{
	struct spi_t * spi = (struct spi_t *)dev->priv;
	struct spi_k210_pdata_t * pdat = (struct spi_k210_pdata_t *)spi->priv;

	if(spi && unregister_spi(spi))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free_device_name(spi->name);
		free(spi->priv);
		free(spi);
	}
}

static void spi_k210_suspend(struct device_t * dev)
{
}

static void spi_k210_resume(struct device_t * dev)
{
}

static struct driver_t spi_k210 = {
	.name		= "spi-k210-xip",
	.probe		= spi_k210_probe,
	.remove		= spi_k210_remove,
	.suspend	= spi_k210_suspend,
	.resume		= spi_k210_resume,
};

static __init void spi_k210_driver_init(void)
{
	register_driver(&spi_k210);
}

static __exit void spi_k210_driver_exit(void)
{
	unregister_driver(&spi_k210);
}

driver_initcall(spi_k210_driver_init);
driver_exitcall(spi_k210_driver_exit);
