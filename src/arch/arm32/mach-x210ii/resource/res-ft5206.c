/*
 * resource/res-ft5206.c
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
#include <ft5206.h>

static struct ft5206_data_t ft5206_data = {
	.i2cbus		= "i2c-gpio.1",
	.addr		= 0x38,
	.rst_pin	= S5PV210_GPH0(6),
};

static struct resource_t res_ft5206 = {
	.name		= "ft5206",
	.id			= -1,
	.data		= &ft5206_data,
};

static __init void resource_rtc_init(void)
{
	if(register_resource(&res_ft5206))
		LOG("Register resource '%s.%d'", res_ft5206.name, res_ft5206.id);
	else
		LOG("Failed to register resource '%s.%d'", res_ft5206.name, res_ft5206.id);
}

static __exit void resource_rtc_exit(void)
{
	if(unregister_resource(&res_ft5206))
		LOG("Unregister resource '%s.%d'", res_ft5206.name, res_ft5206.id);
	else
		LOG("Failed to unregister resource '%s.%d'", res_ft5206.name, res_ft5206.id);
}

resource_initcall(resource_rtc_init);
resource_exitcall(resource_rtc_exit);
