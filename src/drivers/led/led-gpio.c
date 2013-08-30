/*
 * driver/led/led-gpio.c
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
#include <led/led-gpio.h>

struct led_gpio_private_data_t {
	u32_t color;
	struct led_gpio_data_t * rdat;
};

static void led_gpio_init(struct led_t * led)
{
	struct led_gpio_private_data_t * dat = (struct led_gpio_private_data_t *)led->priv;
	struct led_gpio_data_t * rdat = (struct led_gpio_data_t *)dat->rdat;

	gpio_set_pull(rdat->gpio, rdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
}

static void led_gpio_exit(struct led_t * led)
{
	struct led_gpio_private_data_t * dat = (struct led_gpio_private_data_t *)led->priv;
	struct led_gpio_data_t * rdat = (struct led_gpio_data_t *)dat->rdat;

	dat->color = 0;
	gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
}

static void led_gpio_set(struct led_t * led, u32_t color)
{
	struct led_gpio_private_data_t * dat = (struct led_gpio_private_data_t *)led->priv;
	struct led_gpio_data_t * rdat = (struct led_gpio_data_t *)dat->rdat;

	dat->color = color & 0x00ffffff;
	if(dat->color != 0)
		gpio_direction_output(rdat->gpio, rdat->active_low ? 0 : 1);
	else
		gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
}

static u32_t led_gpio_get(struct led_t * led)
{
	struct led_gpio_private_data_t * dat = (struct led_gpio_private_data_t *)led->priv;

	return dat->color;
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
	struct led_gpio_private_data_t * dat;
	struct led_t * led;
	char name[64];

	dat = malloc(sizeof(struct led_gpio_private_data_t));
	if(!dat)
		return FALSE;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->color = 0x0;
	dat->rdat = rdat;

	led->name = strdup(name);
	led->init = led_gpio_init;
	led->exit = led_gpio_exit;
	led->set = led_gpio_set,
	led->get = led_gpio_get,
	led->suspend = led_gpio_suspend,
	led->resume = led_gpio_resume,
	led->priv = dat;

	if(register_led(led))
		return TRUE;

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
	resource_for_each_with_name("led-gpio", led_gpio_register_led);
}

static __exit void led_gpio_device_exit(void)
{
	resource_for_each_with_name("led-gpio", led_gpio_unregister_led);
}

device_initcall(led_gpio_device_init);
device_exitcall(led_gpio_device_exit);
