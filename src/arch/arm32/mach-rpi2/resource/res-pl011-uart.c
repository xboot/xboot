/*
 * resource/res-pl011-uart.c
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
#include <pl011-uart.h>
#include <bcm2836-gpio.h>
#include <bcm2836/reg-uart.h>

static struct pl011_uart_data_t uart_datas[] = {
	[0] = {
		.clk		= "uart0-clk",
		.txdpin		= BCM2836_GPIO(14),
		.txdcfg		= 0,
		.rxdpin		= BCM2836_GPIO(15),
		.rxdcfg		= 0,
		.baud		= B115200,
		.data		= DATA_BITS_8,
		.parity		= PARITY_NONE,
		.stop		= STOP_BITS_1,
		.regbase	= BCM2836_UART0_BASE,
	}
};

static struct resource_t res_uarts[] = {
	{
		.name		= "pl011-uart",
		.id			= 0,
		.data		= &uart_datas[0],
	}
};

static __init void resource_uart_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
		register_resource(&res_uarts[i]);
}
resource_initcall(resource_uart_init);
