/*
 * resource/res-led-gpio.c
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
#include <led/led-gpio.h>
#include <realview-gpio.h>

static struct led_gpio_data_t led_gpio_datas[] = {
	[0] = {
		.gpio		= REALVIEW_GPIO0(0),
		.active_low	= 1,
	}
};

static struct resource_t res_led_gpios[] = {
	{
		.name		= "led-gpio",
		.id			= 0,
		.data		= &led_gpio_datas[0],
	}
};

static __init void resource_led_gpio_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_led_gpios); i++)
		register_resource(&res_led_gpios[i]);
}
resource_initcall(resource_led_gpio_init);
