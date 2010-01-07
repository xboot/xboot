/*
 * devices/dev-serial.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <debug.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <serial/serial.h>
#include <xboot/platform_device.h>


/*
 * serial device.
 */
static struct serial_parameter uart_param[] = {
	[0] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[1] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[2] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	},
	[3] = {
		.baud_rate		= B115200,
		.data_bit		= DATA_BITS_8,
		.parity			= PARITY_NONE,
		.stop_bit		= STOP_BITS_1,
	}
};

/*
 * the array of platform devices.
 */
static struct platform_device serial_devs[] = {
	{
		.name		= "uart0",
		.data		= &uart_param[0],
	}, {
		.name		= "uart1",
		.data		= &uart_param[1],
	}, {
		.name		= "uart2",
		.data		= &uart_param[2],
	}, {
		.name		= "uart3",
		.data		= &uart_param[3],
	}
};

static __init void dev_serial_init(void)
{
	x_u32 i;

	for(i = 0; i < ARRAY_SIZE(serial_devs); i++)
	{
		if(!platform_device_register(&serial_devs[i]))
		{
			DEBUG_E("failed to register platform  device '%s'", serial_devs[i].name);
		}
	}
}

static __exit void dev_serial_exit(void)
{
	x_u32 i;

	for(i = 0; i < ARRAY_SIZE(serial_devs); i++)
	{
		if(!platform_device_unregister(&serial_devs[i]))
		{
			DEBUG_E("failed to unregister platform device '%s'", serial_devs[i].name);
		}
	}
}

module_init(dev_serial_init, LEVEL_MACH_RES);
module_exit(dev_serial_exit, LEVEL_MACH_RES);
