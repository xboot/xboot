/*
 * resource/res-uart.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <realview-uart.h>

static struct realview_uart_data_t realview_uart_data[] = {
	[0] = {
		.baud		= B115200,
		.data		= DATA_BITS_8,
		.parity		= PARITY_NONE,
		.stop		= STOP_BITS_1,
		.regbase	= REALVIEW_UART0_BASE,
	},
	[1] = {
		.baud		= B115200,
		.data		= DATA_BITS_8,
		.parity		= PARITY_NONE,
		.stop		= STOP_BITS_1,
		.regbase	= REALVIEW_UART1_BASE,
	},
	[2] = {
		.baud		= B115200,
		.data		= DATA_BITS_8,
		.parity		= PARITY_NONE,
		.stop		= STOP_BITS_1,
		.regbase	= REALVIEW_UART2_BASE,
	},
	[3] = {
		.baud		= B115200,
		.data		= DATA_BITS_8,
		.parity		= PARITY_NONE,
		.stop		= STOP_BITS_1,
		.regbase	= REALVIEW_UART3_BASE,
	}
};

static struct resource_t res_uarts[] = {
	{
		.name		= "uart.pl011",
		.id			= 0,
		.data		= &realview_uart_data[0],
	}, {
		.name		= "uart.pl011",
		.id			= 1,
		.data		= &realview_uart_data[1],
	}, {
		.name		= "uart.pl011",
		.id			= 2,
		.data		= &realview_uart_data[2],
	}, {
		.name		= "uart.pl011",
		.id			= 3,
		.data		= &realview_uart_data[3],
	}
};

static __init void resource_uart_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
	{
		if(register_resource(&res_uarts[i]))
			LOG("Register resource '%s.%d'", res_uarts[i].name, res_uarts[i].id);
		else
			LOG("Failed to register resource '%s.%d'", res_uarts[i].name, res_uarts[i].id);
	}
}

static __exit void resource_uart_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_uarts); i++)
	{
		if(unregister_resource(&res_uarts[i]))
			LOG("Unregister resource '%s.%d'", res_uarts[i].name, res_uarts[i].id);
		else
			LOG("Failed to unregister resource '%s.%d'", res_uarts[i].name, res_uarts[i].id);
	}
}

resource_initcall(resource_uart_init);
resource_exitcall(resource_uart_exit);
