/*
 * resource/res-led-pwm.c
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
#include <led/led-pwm.h>

static struct led_pwm_data_t led_pwm_datas[] = {
	[0] = {
		.pwm		= "pwm0",
		.period		= 1000000,
		.polarity	= FALSE,
	},
};

static struct resource_t res_led_pwms[] = {
	{
		.name		= "led-pwm",
		.id			= -1,
		.data		= &led_pwm_datas[0],
	},
};

static __init void resource_led_pwm_init(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_led_pwms); i++)
	{
		if(register_resource(&res_led_pwms[i]))
			LOG("Register resource %s:'%s.%d'", res_led_pwms[i].mach, res_led_pwms[i].name, res_led_pwms[i].id);
		else
			LOG("Failed to register resource %s:'%s.%d'", res_led_pwms[i].mach, res_led_pwms[i].name, res_led_pwms[i].id);
	}
}

static __exit void resource_led_pwm_exit(void)
{
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(res_led_pwms); i++)
	{
		if(unregister_resource(&res_led_pwms[i]))
			LOG("Unregister resource %s:'%s.%d'", res_led_pwms[i].mach, res_led_pwms[i].name, res_led_pwms[i].id);
		else
			LOG("Failed to unregister resource %s:'%s.%d'", res_led_pwms[i].mach, res_led_pwms[i].name, res_led_pwms[i].id);
	}
}

resource_initcall(resource_led_pwm_init);
resource_exitcall(resource_led_pwm_exit);
