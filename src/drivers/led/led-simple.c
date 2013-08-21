/*
 * driver/led/led-simple.c
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
#include <led/led-simple.h>

static void led_simple_init(struct led_t * led)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;

	if(dat->init)
		dat->init(dat);
}

static void led_simple_exit(struct led_t * led)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;

	if(dat->set)
		dat->set(dat, 0);
}

static void led_simple_set(struct led_t * led, u32_t color)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;

	if(dat->set)
		return dat->set(dat, color);
}

static void led_simple_suspend(struct led_t * led)
{
}

static void led_simple_resume(struct led_t * led)
{
}

static bool_t led_simple_register_led(struct resource_t * res)
{
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;
	struct led_t * led;
	char name[64];

	led = malloc(sizeof(struct led_t));
	if(!led)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%s", res->name, dat->name);

	led->name = name;
	led->init = led_simple_init;
	led->exit = led_simple_exit;
	led->set = led_simple_set,
	led->suspend = led_simple_suspend,
	led->resume	= led_simple_resume,
	led->priv = res;

	if(register_led(led))
		return TRUE;

	free(led);
	return FALSE;
}

static bool_t led_simple_unregister_led(struct resource_t * res)
{
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%s", res->name, dat->name);

	led = search_led(name);
	if(!led)
		return FALSE;

	if(!unregister_led(led))
		return FALSE;

	free(led);
	return TRUE;
}

static __init void led_simple_device_init(void)
{
	resource_callback_with_name("led.simple", led_simple_register_led);
}

static __exit void led_simple_device_exit(void)
{
	resource_callback_with_name("led.simple", led_simple_unregister_led);
}

device_initcall(led_simple_device_init);
device_exitcall(led_simple_device_exit);
