/*
 * s5p6818-logger.c
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
#include <s5p6818/reg-uart.h>

struct logger_pdata_t {
	physical_addr_t phys;
	virtual_addr_t virt;
};

static void logger_init(struct logger_t * logger)
{
}

static void logger_output(struct logger_t * logger, const char * buf, int count)
{
	struct logger_pdata_t * pdat = (struct logger_pdata_t *)logger->priv;
	int i;

	for(i = 0; i < count; i++)
	{
		while( !(read32(pdat->virt + UART_UTRSTAT) & UART_UTRSTAT_TXFE) );
		write8(pdat->virt + UART_UTXH, buf[i]);
	}
}

static struct logger_pdata_t pdata = {
	.phys	= S5P6818_UART0_BASE,
};

static struct logger_t logger = {
	.name	= "logger-samsung-uart.0",
	.init	= logger_init,
	.output	= logger_output,
	.priv	= &pdata,
};

static __init void s5p6818_logger_init(void)
{
	pdata.virt = phys_to_virt(pdata.phys);
	register_logger(&logger);
}
core_initcall(s5p6818_logger_init);
