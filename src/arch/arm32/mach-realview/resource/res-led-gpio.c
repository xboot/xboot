/*
 * resource/res-led-gpio.c
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
#include <led/led-gpio.h>
#include <realview/reg-gpio.h>

static struct led_gpio_data_t led_gpio_datas[] = {
	[0] = {
		.name		= "left",
		.gpio		= REALVIEW_GPIO0(0),
		.active_low	= 1,
	},
	[1] = {
		.name		= "right",
		.gpio		= REALVIEW_GPIO0(1),
		.active_low	= 1,
	},
};

static struct resource_t res_led_gpios[] = {
	{
		.name		= "led.gpio",
		.id			= -1,
		.data		= &led_gpio_datas[0],
	}, {
		.name		= "led.gpio",
		.id			= -1,
		.data		= &led_gpio_datas[1],
	},
};

static __init void resource_led_gpio_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_led_gpios); i++)
	{
		if(register_resource(&res_led_gpios[i]))
			LOG("Register resource '%s.%d'", res_led_gpios[i].name, res_led_gpios[i].id);
		else
			LOG("Failed to register resource '%s.%d'", res_led_gpios[i].name, res_led_gpios[i].id);
	}
}

static __exit void resource_led_gpio_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_led_gpios); i++)
	{
		if(unregister_resource(&res_led_gpios[i]))
			LOG("Unregister resource '%s.%d'", res_led_gpios[i].name, res_led_gpios[i].id);
		else
			LOG("Failed to unregister resource '%s.%d'", res_led_gpios[i].name, res_led_gpios[i].id);
	}
}

resource_initcall(resource_led_gpio_init);
resource_exitcall(resource_led_gpio_exit);
