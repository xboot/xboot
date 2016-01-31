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
#include <s5p4418-gpio.h>
#include <s5p4418/reg-uart.h>

static struct pl011_uart_data_t uart_datas[] = {
	[0] = {
		.clk	= "GATE-UART0",
		.txdpin	= S5P4418_GPIOD(18),
		.txdcfg	= 0x1,
		.rxdpin	= S5P4418_GPIOD(14),
		.rxdcfg	= 0x1,
		.baud	= 115200,
		.data	= 8,
		.parity	= 0,
		.stop	= 1,
		.phys	= S5P4418_UART0_BASE,
	},
	[1] = {
		.clk	= "GATE-UART1",
		.txdpin	= S5P4418_GPIOD(19),
		.txdcfg	= 0x1,
		.rxdpin	= S5P4418_GPIOD(15),
		.rxdcfg	= 0x1,
		.baud	= 115200,
		.data	= 8,
		.parity	= 0,
		.stop	= 1,
		.phys	= S5P4418_UART1_BASE,
	},
	[2] = {
		.clk	= "GATE-UART2",
		.txdpin	= S5P4418_GPIOD(20),
		.txdcfg	= 0x1,
		.rxdpin	= S5P4418_GPIOD(16),
		.rxdcfg	= 0x1,
		.baud	= 115200,
		.data	= 8,
		.parity	= 0,
		.stop	= 1,
		.phys	= S5P4418_UART2_BASE,
	},
	[3] = {
		.clk	= "GATE-UART3",
		.txdpin	= S5P4418_GPIOD(21),
		.txdcfg	= 0x1,
		.rxdpin	= S5P4418_GPIOD(17),
		.rxdcfg	= 0x1,
		.baud	= 115200,
		.data	= 8,
		.parity	= 0,
		.stop	= 1,
		.phys	= S5P4418_UART3_BASE,
	}
};

static struct resource_t res_uarts[] = {
	{
		.name	= "pl011-uart",
		.id		= 0,
		.data	= &uart_datas[0],
	}, {
		.name	= "pl011-uart",
		.id		= 1,
		.data	= &uart_datas[1],
	}, {
		.name	= "pl011-uart",
		.id		= 2,
		.data	= &uart_datas[2],
	}, {
		.name	= "pl011-uart",
		.id		= 3,
		.data	= &uart_datas[3],
	}
};

static __init void resource_uart_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
		register_resource(&res_uarts[i]);
}
resource_initcall(resource_uart_init);
