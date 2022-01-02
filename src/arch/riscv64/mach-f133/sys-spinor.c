/*
 * sys-spinor.c
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

void sys_spinor_init(void)
{
	virtual_addr_t addr;
	u32_t val;

	/* Config GPIOC2, GPIOC3, GPIOC4 and GPIOC5 */
	addr = 0x02000060 + 0x00;
	val = read32(addr);
	val &= ~(0xf << ((2 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((2 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((3 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((3 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((4 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((4 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((5 & 0x7) << 2));
	val |= ((0x2 & 0xf) << ((5 & 0x7) << 2));
	write32(addr, val);

	/* Deassert spi0 reset */
	addr = 0x0200196c;
	val = read32(addr);
	val |= (1 << 16);
	write32(addr, val);

	/* Open the spi0 gate */
	addr = 0x02001940;
	val = read32(addr);
	val |= (1 << 31);
	write32(addr, val);

	/* Open the spi0 bus gate */
	addr = 0x0200196c;
	val = read32(addr);
	val |= (1 << 0);
	write32(addr, val);

	/* Select pll-periph0 for spi0 clk */
	addr = 0x02001940;
	val = read32(addr);
	val &= ~(0x3 << 24);
	val |= 0x1 << 24;
	write32(addr, val);

	/* Set clock pre divide ratio, divided by 1 */
	addr = 0x02001940;
	val = read32(addr);
	val &= ~(0x3 << 8);
	val |= 0x0 << 8;
	write32(addr, val);

	/* Set clock divide ratio, divided by 6 */
	addr = 0x02001940;
	val = read32(addr);
	val &= ~(0xf << 0);
	val |= (6 - 1) << 0;
	write32(addr, val);

	/* Set spi clock rate control register, divided by 2 */
	addr = 0x04025000;
	write32(addr + SPI_CCR, 0x1000);

	/* Enable spi0 and do a soft reset */
	addr = 0x04025000;
	val = read32(addr + SPI_GCR);
	val |= (1 << 31) | (1 << 7) | (1 << 1) | (1 << 0);
	write32(addr + SPI_GCR, val);
	while(read32(addr + SPI_GCR) & (1 << 31));

	val = read32(addr + SPI_TCR);
	val &= ~(0x3 << 0);
	val |= (1 << 6) | (1 << 2);
	write32(addr + SPI_TCR, val);

	val = read32(addr + SPI_FCR);
	val |= (1 << 31) | (1 << 15);
	write32(addr + SPI_FCR, val);
}

void sys_spinor_exit(void)
{
	virtual_addr_t addr = 0x04025000;
	u32_t val;

	/* Disable the spi0 controller */
	val = read32(addr + SPI_GCR);
	val &= ~((1 << 1) | (1 << 0));
	write32(addr + SPI_GCR, val);
}

static void sys_spi_select(void)
{
	virtual_addr_t addr = 0x04025000;
	u32_t val;

	val = read32(addr + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((0 & 0x3) << 4) | (0x0 << 7);
	write32(addr + SPI_TCR, val);
}

static void sys_spi_deselect(void)
{
	virtual_addr_t addr = 0x04025000;
	u32_t val;

	val = read32(addr + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((0 & 0x3) << 4) | (0x1 << 7);
	write32(addr + SPI_TCR, val);
}

static void sys_spi_write_txbuf(u8_t * buf, int len)
{
	virtual_addr_t addr = 0x04025000;
	int i;

	write32(addr + SPI_MTC, len & 0xffffff);
	write32(addr + SPI_BCC, len & 0xffffff);
	if(buf)
	{
		for(i = 0; i < len; i++)
			write8(addr + SPI_TXD, *buf++);
	}
	else
	{
		for(i = 0; i < len; i++)
			write8(addr + SPI_TXD, 0xff);
	}
}

static int sys_spi_transfer(void * txbuf, void * rxbuf, int len)
{
	virtual_addr_t addr = 0x04025000;
	int count = len;
	u8_t * tx = txbuf;
	u8_t * rx = rxbuf;
	u8_t val;
	int n, i;

	while(count > 0)
	{
		n = (count <= 64) ? count : 64;
		write32(addr + SPI_MBC, n);
		sys_spi_write_txbuf(tx, n);
		write32(addr + SPI_TCR, read32(addr + SPI_TCR) | (1 << 31));

		while((read32(addr + SPI_FSR) & 0xff) < n);
		for(i = 0; i < n; i++)
		{
			val = read8(addr + SPI_RXD);
			if(rx)
				*rx++ = val;
		}

		if(tx)
			tx += n;
		count -= n;
	}
	return len;
}

static int sys_spi_write_then_read(void * txbuf, int txlen, void * rxbuf, int rxlen)
{
	if(sys_spi_transfer(txbuf, NULL, txlen) != txlen)
		return -1;
	if(sys_spi_transfer(NULL, rxbuf, rxlen) != rxlen)
		return -1;
	return 0;
}

void sys_spinor_read(int addr, void * buf, int count)
{
	u8_t tx[4];

	tx[0] = 0x03;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	sys_spi_select();
	sys_spi_write_then_read(tx, 4, buf, count);
	sys_spi_deselect();
}
