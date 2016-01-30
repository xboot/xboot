/*
 * driver/sandbox-led.c
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
#include <sandbox-led.h>

struct sandbox_led_pdata_t
{
	int brightness;
	char * path;
};

static void sandbox_led_init(struct led_t * led)
{
}

static void sandbox_led_exit(struct led_t * led)
{
}

static void sandbox_led_set(struct led_t * led, int brightness)
{
	struct sandbox_led_pdata_t * pdat = (struct sandbox_led_pdata_t *)led->priv;

	if(pdat->brightness != brightness)
	{
		sandbox_sysfs_write_value(pdat->path, brightness);
		pdat->brightness = brightness;
	}
}

static int sandbox_led_get(struct led_t * led)
{
	struct sandbox_led_pdata_t * pdat = (struct sandbox_led_pdata_t *)led->priv;
	return pdat->brightness;
}

static void sandbox_led_suspend(struct led_t * led)
{
}

static void sandbox_led_resume(struct led_t * led)
{
}

static bool_t sandbox_register_led(struct resource_t * res)
{
	struct sandbox_led_data_t * rdat = (struct sandbox_led_data_t *)res->data;
	struct sandbox_led_pdata_t * pdat;
	struct led_t * led;
	char name[64];

	pdat = malloc(sizeof(struct sandbox_led_pdata_t));
	if(!pdat)
		return FALSE;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->brightness = 0;
	pdat->path = strdup(rdat->path);

	led->name = strdup(name);
	led->init = sandbox_led_init;
	led->exit = sandbox_led_exit;
	led->set = sandbox_led_set,
	led->get = sandbox_led_get,
	led->suspend = sandbox_led_suspend,
	led->resume	= sandbox_led_resume,
	led->priv = pdat;

	if(register_led(led))
		return TRUE;

	free(led->priv);
	free(led->name);
	free(led);
	return FALSE;
}

static bool_t sandbox_unregister_led(struct resource_t * res)
{
	struct sandbox_led_pdata_t * pdat;
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	led = search_led(name);
	if(!led)
		return FALSE;
	pdat = (struct sandbox_led_pdata_t *)led->priv;

	if(!unregister_led(led))
		return FALSE;

	free(pdat->path);
	free(led->priv);
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
