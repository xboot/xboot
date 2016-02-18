/*
 * driver/led/led-gpio.c
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

#include <led/led-gpio.h>

struct led_gpio_pdata_t {
	int gpio;
	int active_low;
	int brightness;
};

static void led_gpio_init(struct led_t * led)
{
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	if(pdat->brightness > 0)
		gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
	else
		gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void led_gpio_exit(struct led_t * led)
{
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	pdat->brightness = 0;
	gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void led_gpio_set(struct led_t * led, int brightness)
{
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	if(pdat->brightness != brightness)
	{
		if(brightness > 0)
			gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
		else
			gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
		pdat->brightness = brightness;
	}
}

static int led_gpio_get(struct led_t * led)
{
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;
	return pdat->brightness;
}

static void led_gpio_suspend(struct led_t * led)
{
}

static void led_gpio_resume(struct led_t * led)
{
}

static bool_t led_gpio_register_led(struct resource_t * res)
{
	struct led_gpio_data_t * rdat = (struct led_gpio_data_t *)res->data;
	struct led_gpio_pdata_t * pdat;
	struct led_t * led;
	char name[64];

	pdat = malloc(sizeof(struct led_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->gpio = rdat->gpio;
	pdat->active_low = rdat->active_low;
	pdat->brightness = 0;

	led->name = strdup(name);
	led->init = led_gpio_init;
	led->exit = led_gpio_exit;
	led->set = led_gpio_set,
	led->get = led_gpio_get,
	led->suspend = led_gpio_suspend,
	led->resume = led_gpio_resume,
	led->priv = pdat;

	if(register_led(led))
		return TRUE;

	free(led->priv);
	free(led->name);
	free(led);
	return FALSE;
}

static bool_t led_gpio_unregister_led(struct resource_t * res)
{
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	led = search_led(name);
	if(!led)
		return FALSE;

	if(!unregister_led(led))
		return FALSE;

	free(led->priv);
	free(led->name);
	free(led);
	return TRUE;
}

static __init void led_gpio_device_init(void)
{
	resource_for_each("led-gpio", led_gpio_register_led);
}

static __exit void led_gpio_device_exit(void)
{
	resource_for_each("led-gpio", led_gpio_unregister_led);
}

device_initcall(led_gpio_device_init);
device_exitcall(led_gpio_device_exit);
