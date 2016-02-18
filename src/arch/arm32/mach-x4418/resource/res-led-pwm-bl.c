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
#include <led/led-pwm-bl.h>
#include <s5p4418-gpio.h>

static struct led_pwm_bl_data_t led_pwm_bl_datas[] = {
	[0] = {
		.pwm				= "pwm0",
		.period				= 1000 * 1000,
		.from				= 6,
		.to					= 100,
		.polarity			= 1,
		.power				= S5P4418_GPIOC(10),
		.power_active_low	= 0,
	}
};

static struct resource_t res_led_pwm_bls[] = {
	{
		.name	= "led-pwm-bl",
		.id		= 0,
		.data	= &led_pwm_bl_datas[0],
	}
};

static __init void resource_led_pwm_bl_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_led_pwm_bls); i++)
		register_resource(&res_led_pwm_bls[i]);
}
resource_initcall(resource_led_pwm_bl_init);
