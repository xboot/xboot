/*
 * resource/res-ledtrig-general.c
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
#include <led/ledtrig.h>

static struct led_trigger_data_t led_trigger_datas[] = {
	{
		.name	= "console",
		.led	= "led.simple.d1",
	},
};

static struct resource_t res_led_triggers[] = {
	{
		.name	= "ledtrig.general",
		.id		= -1,
		.data	= &led_trigger_datas[0],
	},
};

static __init void resource_led_trigger_general_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_led_triggers); i++)
	{
		if(register_resource(&res_led_triggers[i]))
			LOG("Register resource '%s.%d'", res_led_triggers[i].name, res_led_triggers[i].id);
		else
			LOG("Failed to register resource '%s.%d'", res_led_triggers[i].name, res_led_triggers[i].id);
	}
}

static __exit void resource_led_trigger_general_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_led_triggers); i++)
	{
		if(unregister_resource(&res_led_triggers[i]))
			LOG("Unregister resource '%s.%d'", res_led_triggers[i].name, res_led_triggers[i].id);
		else
			LOG("Failed to unregister resource '%s.%d'", res_led_triggers[i].name, res_led_triggers[i].id);
	}
}

resource_initcall(resource_led_trigger_general_init);
resource_exitcall(resource_led_trigger_general_exit);
