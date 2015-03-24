/*
 * driver/sandbox-led.c
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
#include <sandbox-led.h>

static void sandbox_led_init(struct led_t * led)
{
}

static void sandbox_led_exit(struct led_t * led)
{
}

static void sandbox_led_set(struct led_t * led, int brightness)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct sandbox_led_data_t * dat = (struct sandbox_led_data_t *)res->data;
	sandbox_sysfs_write_value(dat->path, brightness);
}

static int sandbox_led_get(struct led_t * led)
{
	struct resource_t * res = (struct resource_t *)led->priv;
	struct sandbox_led_data_t * dat = (struct sandbox_led_data_t *)res->data;
	int brightness = 0;

	sandbox_sysfs_read_value(dat->path, &brightness);
	return brightness;
}

static void sandbox_led_suspend(struct led_t * led)
{
}

static void sandbox_led_resume(struct led_t * led)
{
}

static bool_t sandbox_register_led(struct resource_t * res)
{
	struct led_t * led;
	char name[64];

	led = malloc(sizeof(struct led_t));
	if(!led)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	led->name = strdup(name);
	led->init = sandbox_led_init;
	led->exit = sandbox_led_exit;
	led->set = sandbox_led_set,
	led->get = sandbox_led_get,
	led->suspend = sandbox_led_suspend,
	led->resume	= sandbox_led_resume,
	led->priv = res;

	if(register_led(led))
		return TRUE;

	free(led->name);
	free(led);
	return FALSE;
}

static bool_t sandbox_unregister_led(struct resource_t * res)
{
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	led = search_led(name);
	if(!led)
		return FALSE;

	if(!unregister_led(led))
		return FALSE;

	free(led->name);
	free(led);
	return TRUE;
}

static __init void sandbox_led_device_init(void)
{
	resource_for_each_with_name("sandbox-led", sandbox_register_led);
}

static __exit void sandbox_led_device_exit(void)
{
	resource_for_each_with_name("sandbox-led", sandbox_unregister_led);
}

device_initcall(sandbox_led_device_init);
device_exitcall(sandbox_led_device_exit);
