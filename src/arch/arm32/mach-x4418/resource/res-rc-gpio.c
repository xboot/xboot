/*
 * resource/res-rc-gpio.c
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
#include <input/rc/rc-gpio.h>
#include <s5p4418-gpio.h>

static struct rc_map_t rc_gpio_maps[] = {
	{ 0x0040ff01, KEY_1 },
	{ 0x0040ff02, KEY_2 },
	{ 0x0040ff03, KEY_3 },
	{ 0x0040ff04, KEY_4 },
	{ 0x0040ff05, KEY_5 },
	{ 0x0040ff06, KEY_6 },
	{ 0x0040ff07, KEY_7 },
	{ 0x0040ff08, KEY_8 },
	{ 0x0040ff09, KEY_9 },
	{ 0x0040ff00, KEY_0 },
	{ 0x0040ff12, KEY_DELETE },

	{ 0x0040ff4d, KEY_POWER },
	{ 0x0040ff0b, KEY_UP },
	{ 0x0040ff0e, KEY_DOWN },
	{ 0x0040ff10, KEY_LEFT },
	{ 0x0040ff11, KEY_RIGHT },
	{ 0x0040ff0d, KEY_ENTER },
	{ 0x0040ff1a, KEY_HOME },
	{ 0x0040ff45, KEY_MENU },
	{ 0x0040ff42, KEY_BACK },

	{ 0x0040ff43, KEY_TAB },
	{ 0x0040ff15, KEY_VOLUME_UP },
	{ 0x0040ff1c, KEY_VOLUME_DOWN },

	{ 0x0040ff50, KEY_END },
	{ 0x0040ff46, KEY_PAGE_UP },
	{ 0x0040ff74, KEY_PAGE_DOWN },
};

static struct rc_gpio_data_t rc_gpio_datas[] = {
	[0] = {
		.map		= rc_gpio_maps,
		.size		= ARRAY_SIZE(rc_gpio_maps),
		.gpio		= S5P4418_GPIOD(8),
		.active_low	= 1,
	}
};

static struct resource_t res_rc_gpios[] = {
	{
		.name		= "rc-gpio",
		.id			= 0,
		.data		= &rc_gpio_datas[0],
	}
};

static __init void resource_rc_gpio_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_rc_gpios); i++)
		register_resource(&res_rc_gpios[i]);
}
resource_initcall(resource_rc_gpio_init);
