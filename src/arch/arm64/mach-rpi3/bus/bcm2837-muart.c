/*
 * bus/bcm2837-muart.c
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

#include <bcm2837-muart.h>

struct bcm2837_muart_pdata_t {
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
	virtual_addr_t virtaux;
	virtual_addr_t virtmu;
};

static bool_t bcm2837_muart_set(struct uart_t * uart, int baud, int data, int parity, int stop)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;
	u32_t divider, remainder, fraction, val;
	u8_t dreg, preg, sreg;
	u64_t uclk;

	if(baud < 0)
		return FALSE;
	if((data < 5) || (data > 8))
		return FALSE;
	if((parity < 0) || (parity > 2))
		return FALSE;
	if((stop < 0) || (stop > 2))
		return FALSE;

	switch(data)
	{
	case 5:	/* Data bits = 5 */
		dreg = 0x0;
		break;
	case 6:	/* Data bits = 6 */
		dreg = 0x1;
		break;
	case 7:	/* Data bits = 7 */
		dreg = 0x2;
		break;
	case 8:	/* Data bits = 8 */
		dreg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case 0:	/* Parity none */
		preg = 0x0;
		break;
	case 1:	/* Parity odd */
		preg = 0x1;
		break;
	case 2:	/* Parity even */
		preg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case 1:	/* Stop bits = 1 */
		sreg = 0;
		break;
	case 2:	/* Stop bits = 2 */
		sreg = 1;
		break;
	case 0:	/* Stop bits = 1.5 */
	default:
		return FALSE;
	}

	pdat->baud = baud;
	pdat->data = data;
	pdat->parity = parity;
	pdat->stop = stop;

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	uclk = clk_get_rate(pdat->clk);
	divider = uclk / (16 * baud);
	remainder = uclk % (16 * baud);
	fraction = (8 * remainder / baud) >> 1;
	fraction += (8 * remainder / baud) & 1;

	return TRUE;
}

static bool_t bcm2837_muart_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;

	if(baud)
		*baud = pdat->baud;
	if(data)
		*data = pdat->data;
	if(parity)
		*parity = pdat->parity;
	if(stop)
		*stop = pdat->stop;
	return TRUE;
}

static void bcm2837_muart_init(struct uart_t * uart)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;

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
	bcm2837_muart_set(uart, pdat->baud, pdat->data, pdat->parity, pdat->stop);
}

static void bcm2837_muart_exit(struct uart_t * uart)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;
	clk_disable(pdat->clk);
}

static ssize_t bcm2837_muart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if((read8(pdat->virtmu + AUX_MU_LSR) & 0x01))
			buf[i] = read8(pdat->virtmu + AUX_MU_IO);
		else
			break;
	}
	return i;
}

static ssize_t bcm2837_muart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct bcm2837_muart_pdata_t * pdat = (struct bcm2837_muart_pdata_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while(!(read8(pdat->virtmu + AUX_MU_LSR) & 0x20));
		write8(pdat->virtmu + AUX_MU_IO, buf[i]);
	}
	return i;
}

static bool_t bcm2837_register_bus_uart(struct resource_t * res)
{
	struct bcm2837_muart_data_t * rdat = (struct bcm2837_muart_data_t *)res->data;
	struct bcm2837_muart_pdata_t * pdat;
	struct uart_t * uart;
	char name[64];

	if(!clk_search(rdat->clk))
		return FALSE;

	pdat = malloc(sizeof(struct bcm2837_muart_pdata_t));
	if(!pdat)
		return FALSE;

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->clk = strdup(rdat->clk);
	pdat->txdpin = rdat->txdpin;
	pdat->txdcfg = rdat->txdcfg;
	pdat->rxdpin = rdat->rxdpin;
	pdat->rxdcfg = rdat->rxdcfg;
	pdat->baud = rdat->baud;
	pdat->data = rdat->data;
	pdat->parity = rdat->parity;
	pdat->stop = rdat->stop;
	pdat->virtaux = phys_to_virt(rdat->physaux);
	pdat->virtmu = phys_to_virt(rdat->physmu);

	uart->name = strdup(name);
	uart->init = bcm2837_muart_init;
	uart->exit = bcm2837_muart_exit;
	uart->set = bcm2837_muart_set;
	uart->get = bcm2837_muart_get;
	uart->read = bcm2837_muart_read;
	uart->write = bcm2837_muart_write;
	uart->priv = pdat;

	if(register_bus_uart(uart))
		return TRUE;

	free(pdat->clk);
	free(uart->priv);
	free(uart->name);
	free(uart);
	return FALSE;
}

static bool_t bcm2837_unregister_bus_uart(struct resource_t * res)
{
	struct bcm2837_muart_pdata_t * pdat;
	struct uart_t * uart;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;
	pdat = (struct bcm2837_muart_pdata_t *)uart->priv;

	if(!unregister_bus_uart(uart))
		return FALSE;

	free(pdat->clk);
	free(uart->priv);
	free(uart->name);
	free(uart);
	return TRUE;
}

static __init void bcm2837_bus_uart_init(void)
{
	resource_for_each("bcm2837-muart", bcm2837_register_bus_uart);
}

static __exit void bcm2837_bus_uart_exit(void)
{
	resource_for_each("bcm2837-muart", bcm2837_unregister_bus_uart);
}

bus_initcall(bcm2837_bus_uart_init);
bus_exitcall(bcm2837_bus_uart_exit);
