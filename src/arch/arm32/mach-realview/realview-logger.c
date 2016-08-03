/*
 * realview-logger.c
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

#define UART_DATA	(0x00)
#define UART_RSR	(0x04)
#define UART_FR		(0x18)
#define UART_ILPR	(0x20)
#define UART_IBRD	(0x24)
#define UART_FBRD	(0x28)
#define UART_LCRH	(0x2c)
#define UART_CR		(0x30)
#define UART_IFLS	(0x34)
#define UART_IMSC	(0x38)
#define UART_RIS	(0x3c)
#define UART_MIS	(0x40)
#define UART_ICR	(0x44)
#define UART_DMACR	(0x48)

struct logger_pdata_t {
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	physical_addr_t phys;
	virtual_addr_t virt;
};

static void logger_init(struct logger_t * logger)
{
	struct logger_pdata_t * pdat = (struct logger_pdata_t *)logger->priv;
	u32_t div, rem, frac;

	clk_enable(pdat->clk);
	if(pdat->txdpin >= 0)
	{
		gpio_set_cfg(pdat->txdpin, pdat->txdcfg);
		gpio_set_pull(pdat->txdpin, GPIO_PULL_UP);
	}
	if(pdat->rxdpin >= 0)
	{
		gpio_set_cfg(pdat->rxdpin, pdat->rxdcfg);
		gpio_set_pull(pdat->rxdpin, GPIO_PULL_UP);
	}

	div = clk_get_rate(pdat->clk) / (16 * 115200);
	rem = clk_get_rate(pdat->clk) % (16 * 115200);
	frac = (8 * rem / 115200) >> 1;
	frac += (8 * rem / 115200) & 1;

	write32(pdat->virt + UART_IBRD, div);
	write32(pdat->virt + UART_FBRD, frac);
	write32(pdat->virt + UART_LCRH, (0x3 << 5) | (0x0 << 3) | (0x0 << 1) | (0x1 << 4));
	write32(pdat->virt + UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

static void logger_output(struct logger_t * logger, const char * buf, int count)
{
	struct logger_pdata_t * pdat = (struct logger_pdata_t *)logger->priv;
	int i;

	for(i = 0; i < count; i++)
	{
		while((read8(pdat->virt + UART_FR) & (0x1 << 5)));
		write8(pdat->virt + UART_DATA, buf[i]);
	}
}

static struct logger_pdata_t pdata = {
	.clk	= "uclk",
	.txdpin	= -1,
	.txdcfg	= -1,
	.rxdpin	= -1,
	.rxdcfg	= -1,
	.phys	= 0x10009000,
};

static struct logger_t logger = {
	.name	= "logger-pl110-uart.0",
	.init	= logger_init,
	.output	= logger_output,
	.priv	= &pdata,
};

static __init void realview_logger_init(void)
{
	pdata.virt = phys_to_virt(pdata.phys);
	register_logger(&logger);
}
postcore_initcall(realview_logger_init);
