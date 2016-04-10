/*
 * bcm2837-logger.c
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
#include <bcm2837-gpio.h>
#include <bcm2837/reg-aux.h>

struct logger_pdata_t {
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	physical_addr_t physaux;
	physical_addr_t physmu;
	virtual_addr_t virtaux;
	virtual_addr_t virtmu;
};

static void logger_init(struct logger_t * logger)
{
	struct logger_pdata_t * pdat = (struct logger_pdata_t *)logger->priv;

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

	write32(pdat->virtaux + AUX_ENB, read32(pdat->virtaux + AUX_ENB) | (1 << 0));
	write32(pdat->virtmu + AUX_MU_CNTL, 0);
	write32(pdat->virtmu + AUX_MU_IER, 0);
	write32(pdat->virtmu + AUX_MU_IIR, 0xc6);
	write32(pdat->virtmu + AUX_MU_LCR, 3);
	write32(pdat->virtmu + AUX_MU_MCR, 0);
	write32(pdat->virtmu + AUX_MU_BAUD, (clk_get_rate("core-clk") / (8 * 115200) - 1));
	write32(pdat->virtmu + AUX_MU_CNTL, 0x3);
}

static void logger_output(struct logger_t * logger, const char * buf, int count)
{
	struct logger_pdata_t * pdat = (struct logger_pdata_t *)logger->priv;
	int i;

	for(i = 0; i < count; i++)
	{
		while(!(read8(pdat->virtmu + AUX_MU_LSR) & 0x20));
		write8(pdat->virtmu + AUX_MU_IO, buf[i]);
	}
}

static struct logger_pdata_t pdata = {
	.clk		= "core-clk",
	.txdpin		= BCM2837_GPIO(14),
	.txdcfg		= 5,
	.rxdpin		= BCM2837_GPIO(15),
	.rxdcfg		= 5,
	.physaux	= BCM2837_AUX_BASE,
	.physmu		= BCM2837_AUX_MU_BASE,
};

static struct logger_t logger = {
	.name	= "logger-bcm2837-muart.0",
	.init	= logger_init,
	.output	= logger_output,
	.priv	= &pdata,
};

static __init void bcm2837_logger_init(void)
{
	pdata.virtaux = phys_to_virt(pdata.physaux);
	pdata.virtmu = phys_to_virt(pdata.physmu);
	register_logger(&logger);
}
postcore_initcall(bcm2837_logger_init);
