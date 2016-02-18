/*
 * driver/led/led-pwm-bl.c
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

#include <led/led-pwm-bl.h>

struct led_pwm_bl_pdata_t {
	struct pwm_t * pwm;
	u32_t period;
	u32_t dutyfrom;
	u32_t dutyto;
	int polarity;
	int power;
	int power_active_low;
	int brightness;
};

static void led_pwm_bl_init(struct led_t * led)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;

	gpio_set_pull(pdat->power, pdat->power_active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	if(pdat->brightness > 0)
	{
		u32_t duty = pdat->dutyfrom + (pdat->dutyto - pdat->dutyfrom) * pdat->brightness / (CONFIG_MAX_BRIGHTNESS + 1);
		pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
		pwm_enable(pdat->pwm);
		gpio_direction_output(pdat->power, pdat->power_active_low ? 0 : 1);
	}
	else
	{
		pwm_disable(pdat->pwm);
		gpio_direction_output(pdat->power, pdat->power_active_low ? 1 : 0);
	}
}

static void led_pwm_bl_exit(struct led_t * led)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;
	pdat->brightness = 0;
	pwm_disable(pdat->pwm);
	gpio_direction_output(pdat->power, pdat->power_active_low ? 1 : 0);
}

static void led_pwm_bl_set(struct led_t * led, int brightness)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;

	if(pdat->brightness != brightness)
	{
		if(brightness > 0)
		{
			u32_t duty = pdat->dutyfrom + (pdat->dutyto - pdat->dutyfrom) * brightness / (CONFIG_MAX_BRIGHTNESS + 1);
			pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
			pwm_enable(pdat->pwm);
			gpio_direction_output(pdat->power, pdat->power_active_low ? 0 : 1);
		}
		else
		{
			pwm_disable(pdat->pwm);
			gpio_direction_output(pdat->power, pdat->power_active_low ? 1 : 0);
		}
		pdat->brightness = brightness;
	}
}

static int led_pwm_bl_get(struct led_t * led)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;
	return pdat->brightness;
}

static void led_pwm_bl_suspend(struct led_t * led)
{
}

static void led_pwm_bl_resume(struct led_t * led)
{
}

static bool_t led_pwm_bl_register_led(struct resource_t * res)
{
	struct led_pwm_bl_data_t * rdat = (struct led_pwm_bl_data_t *)res->data;
	struct led_pwm_bl_pdata_t * pdat;
	struct led_t * led;
	struct pwm_t * pwm;
	char name[64];

	pwm = search_pwm(rdat->pwm);
	if(!pwm)
		return FALSE;

	pdat = malloc(sizeof(struct led_pwm_bl_pdata_t));
	if(!pdat)
		return FALSE;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->pwm = pwm;
	pdat->period = rdat->period;
	pdat->dutyfrom = rdat->from * rdat->period / 100;
	pdat->dutyto = rdat->to * rdat->period / 100;
	pdat->polarity = rdat->polarity;
	pdat->power = rdat->power;
	pdat->power_active_low = rdat->power_active_low;
	pdat->brightness = 0;

	led->name = strdup(name);
	led->init = led_pwm_bl_init;
	led->exit = led_pwm_bl_exit;
	led->set = led_pwm_bl_set,
	led->get = led_pwm_bl_get,
	led->suspend = led_pwm_bl_suspend,
	led->resume = led_pwm_bl_resume,
	led->priv = pdat;

	if(register_led(led))
		return TRUE;

	free(led->priv);
	free(led->name);
	free(led);
	return FALSE;
}

static bool_t led_pwm_bl_unregister_led(struct resource_t * res)
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

static __init void led_pwm_bl_device_init(void)
{
	resource_for_each_with_name("led-pwm-bl", led_pwm_bl_register_led);
}

static __exit void led_pwm_bl_device_exit(void)
{
	resource_for_each_with_name("led-pwm-bl", led_pwm_bl_unregister_led);
}

device_initcall(led_pwm_bl_device_init);
device_exitcall(led_pwm_bl_device_exit);
