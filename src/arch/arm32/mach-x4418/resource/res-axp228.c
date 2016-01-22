/*
 * resource/res-axp228.c
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

#include <axp228.h>

static struct axp228_data_t axp228_data = {
	.i2cbus		= "i2c-gpio.3",
	.addr		= 0x34,

	.dcdc1		= 3300,
	.dcdc2		= 1100,
	.dcdc3		= 1100,
	.dcdc4		= 1500,
	.dcdc5		= 1500,
	.dc5ldo		= 1200,

	.aldo1		= 3300,
	.aldo2		= 1800,
	.aldo3		= 1000,

	.dldo1		= 3300,
	.dldo2		= 1800,
	.dldo3		= 700,
	.dldo4		= 700,

	.eldo1		= 1800,
	.eldo2		= 3300,
	.eldo3		= 700,
};

static struct resource_t res_axp228 = {
	.name		= "axp228",
	.id			= -1,
	.data		= &axp228_data,
};

static __init void resource_pmic_init(void)
{
	if(register_resource(&res_axp228))
		LOG("Register resource '%s:%s.%d'", res_axp228.mach, res_axp228.name, res_axp228.id);
	else
		LOG("Failed to register resource '%s:%s.%d'", res_axp228.mach, res_axp228.name, res_axp228.id);
}

static __exit void resource_pmic_exit(void)
{
	if(unregister_resource(&res_axp228))
		LOG("Unregister resource '%s:%s.%d'", res_axp228.mach, res_axp228.name, res_axp228.id);
	else
		LOG("Failed to unregister resource '%s:%s.%d'", res_axp228.mach, res_axp228.name, res_axp228.id);
}

resource_initcall(resource_pmic_init);
resource_exitcall(resource_pmic_exit);
