/*
 * driver/led/leddev.c
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

static void led_init(struct led_t * led)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_data_t * dat = (struct led_data_t *)res->data;

	if(dat->init)
		dat->init(dat);
}

static void led_exit(struct led_t * led)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_data_t * dat = (struct led_data_t *)res->data;

	if(dat->set)
		dat->set(dat, 0);
}

static void led_set(struct led_t * led, int brightness)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct led_data_t * dat = (struct led_data_t *)res->data;

	if(brightness < 0)
		brightness = 0;

	if(brightness > 255)
		brightness = 255;

	if(dat->set)
		return dat->set(dat, brightness);
}

static void led_suspend(struct led_t * led)
{
}

static void led_resume(struct led_t * led)
{
}

static bool_t leddev_register_led(struct resource_t * res)
{
	struct led_t * led;
	char name[64];

	led = malloc(sizeof(struct led_t));
	if(!led)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);
	led->name = name;
	led->init = led_init;
	led->exit = led_exit;
	led->set = led_set,
	led->suspend = led_suspend,
	led->resume	= led_resume,
	led->priv = res;

	if(!register_led(led))
		return FALSE;
	return TRUE;
}

static bool_t leddev_unregister_led(struct resource_t * res)
{
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	led = search_led(name);
	if(!led)
		return FALSE;

	return unregister_led(led);
}

static __init void led_device_init(void)
{
	resource_callback_with_name("led", leddev_register_led);
}

static __exit void led_device_exit(void)
{
	resource_callback_with_name("led", leddev_unregister_led);
}

device_initcall(led_device_init);
device_exitcall(led_device_exit);
