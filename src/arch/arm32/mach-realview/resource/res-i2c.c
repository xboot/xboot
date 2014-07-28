/*
 * resource/res-i2c.c
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
#include <realview-i2c.h>

static struct realview_i2c_data_t i2c_datas[] = {
	{
		.udelay		= 50,
		.timeout	= 100,
		.regbase	= REALVIEW_I2C_BASE,
	}
};

static struct resource_t res_i2cs[] = {
	{
		.name		= "realview-i2c",
		.id			= 0,
		.data		= &i2c_datas[0],
	}
};

static __init void resource_realview_i2c_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_i2cs); i++)
	{
		if(register_resource(&res_i2cs[i]))
			LOG("Register resource '%s.%d'", res_i2cs[i].name, res_i2cs[i].id);
		else
			LOG("Failed to register resource '%s.%d'", res_i2cs[i].name, res_i2cs[i].id);
	}
}

static __exit void resource_realview_i2c_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_i2cs); i++)
	{
		if(unregister_resource(&res_i2cs[i]))
			LOG("Unregister resource '%s.%d'", res_i2cs[i].name, res_i2cs[i].id);
		else
			LOG("Failed to unregister resource '%s.%d'", res_i2cs[i].name, res_i2cs[i].id);
	}
}

resource_initcall(resource_realview_i2c_init);
resource_exitcall(resource_realview_i2c_exit);
