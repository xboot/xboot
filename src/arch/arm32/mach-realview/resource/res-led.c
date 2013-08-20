/*
 * resource/res-led.c
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
#include <led/leddev.h>

static void led0_init(struct led_data_t * dat)
{
}

static void led0_set(struct led_data_t * dat, int brightness)
{
}

static void led1_init(struct led_data_t * dat)
{
}

static void led1_set(struct led_data_t * dat, int brightness)
{
}

static struct led_data_t led_datas[] = {
	[0] = {
		.init		= led0_init,
		.set		= led0_set,
	},
	[1] = {
		.init		= led1_init,
		.set		= led1_set,
	},
};

static struct resource_t res_leds[] = {
	{
		.name		= "led",
		.id			= 0,
		.data		= &led_datas[0],
	}, {
		.name		= "led",
		.id			= 1,
		.data		= &led_datas[1],
	},
};

static __init void resource_led_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_leds); i++)
	{
		if(register_resource(&res_leds[i]))
			LOG("Register resource '%s.%d'", res_leds[i].name, res_leds[i].id);
		else
			LOG("Failed to register resource '%s.%d'", res_leds[i].name, res_leds[i].id);
	}
}

static __exit void resource_led_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_leds); i++)
	{
		if(unregister_resource(&res_leds[i]))
			LOG("Unregister resource '%s.%d'", res_leds[i].name, res_leds[i].id);
		else
			LOG("Failed to unregister resource '%s.%d'", res_leds[i].name, res_leds[i].id);
	}
}

resource_initcall(resource_led_init);
resource_exitcall(resource_led_exit);
