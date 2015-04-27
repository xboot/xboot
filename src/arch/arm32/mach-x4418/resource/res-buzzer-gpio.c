/*
 * resource/res-buzzer-gpio.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <buzzer/buzzer-gpio.h>
#include <s5p4418/reg-gpio.h>

static struct buzzer_gpio_data_t buzzer_gpio_data = {
	.gpio		= S5P4418_GPIOC(14),
	.active_low	= 0,
};

static struct resource_t res_buzzer_gpio = {
	.name		= "buzzer-gpio",
	.id			= -1,
	.data		= &buzzer_gpio_data,
};

static __init void resource_buzzer_gpio_init(void)
{
	if(register_resource(&res_buzzer_gpio))
		LOG("Register resource %s:'%s.%d'", res_buzzer_gpio.mach, res_buzzer_gpio.name, res_buzzer_gpio.id);
	else
		LOG("Failed to register resource %s:'%s.%d'", res_buzzer_gpio.mach, res_buzzer_gpio.name, res_buzzer_gpio.id);
}

static __exit void resource_buzzer_gpio_exit(void)
{
	if(unregister_resource(&res_buzzer_gpio))
		LOG("Unregister resource %s:'%s.%d'", res_buzzer_gpio.mach, res_buzzer_gpio.name, res_buzzer_gpio.id);
	else
		LOG("Failed to unregister resource %s:'%s.%d'", res_buzzer_gpio.mach, res_buzzer_gpio.name, res_buzzer_gpio.id);
}

resource_initcall(resource_buzzer_gpio_init);
resource_exitcall(resource_buzzer_gpio_exit);
