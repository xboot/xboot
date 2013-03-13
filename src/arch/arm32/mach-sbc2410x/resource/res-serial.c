/*
 * resource/res-serial.c
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
#include <types.h>
#include <fb/fb.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <serial/serial.h>
#include <xboot/resource.h>

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
	}
};

/*
 * the array of resource.
 */
static struct resource s3c2410_devs[] = {
	{
		.name		= "uart0",
		.data		= &uart_param[0],
	}, {
		.name		= "uart1",
		.data		= &uart_param[1],
	}, {
		.name		= "uart2",
		.data		= &uart_param[2],
	}
};

static __init void s3c2410_devs_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s3c2410_devs); i++)
	{
		if(!register_resource(&s3c2410_devs[i]))
		{
			LOG_E("failed to register resource '%s'", s3c2410_devs[i].name);
		}
	}
}

static __exit void s3c2410_devs_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(s3c2410_devs); i++)
	{
		if(!unregister_resource(&s3c2410_devs[i]))
		{
			LOG_E("failed to unregister resource '%s'", s3c2410_devs[i].name);
		}
	}
}

core_initcall(s3c2410_devs_init);
core_exitcall(s3c2410_devs_exit);
