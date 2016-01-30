/*
 * driver/led/led-pwm.c
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

#include <led/led-pwm.h>

struct led_pwm_pdata_t {
	int brightness;
	u32_t period;
	bool_t polarity;
	struct pwm_t * pwm;
};

static void led_pwm_init(struct led_t * led)
{
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;
	pdat->brightness = 0;
	pwm_disable(pdat->pwm);
}

static void led_pwm_exit(struct led_t * led)
{
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;
	pdat->brightness = 0;
	pwm_disable(pdat->pwm);
}

static void led_pwm_set(struct led_t * led, int brightness)
{
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;
	u32_t duty;

	if(pdat->brightness != brightness)
	{
		if(brightness > 0)
		{
			duty = pdat->brightness * pdat->period / (CONFIG_MAX_BRIGHTNESS + 1);
			pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
			pwm_enable(pdat->pwm);
		}
		else
		{
			pwm_disable(pdat->pwm);
		}
		pdat->brightness = brightness;
	}
}

static int led_pwm_get(struct led_t * led)
{
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;
	return pdat->brightness;
}

static void led_pwm_suspend(struct led_t * led)
{
}

static void led_pwm_resume(struct led_t * led)
{
}

static bool_t led_pwm_register_led(struct resource_t * res)
{
	struct led_pwm_data_t * rdat = (struct led_pwm_data_t *)res->data;
	struct led_pwm_pdata_t * pdat;
	struct led_t * led;
	struct pwm_t * pwm;
	char name[64];

	pwm = search_pwm(rdat->pwm);
	if(!pwm)
		return FALSE;

	pdat = malloc(sizeof(struct led_pwm_pdata_t));
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
	pdat->period = rdat->period;
	pdat->polarity = rdat->polarity;
	pdat->pwm = pwm;

	led->name = strdup(name);
	led->init = led_pwm_init;
	led->exit = led_pwm_exit;
	led->set = led_pwm_set,
	led->get = led_pwm_get,
	led->suspend = led_pwm_suspend,
	led->resume = led_pwm_resume,
	led->priv = pdat;

	if(register_led(led))
		return TRUE;

	free(led->priv);
	free(led->name);
	free(led);
	return FALSE;
}

static bool_t led_pwm_unregister_led(struct resource_t * res)
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

static __init void led_pwm_device_init(void)
{
	resource_for_each_with_name("led-pwm", led_pwm_register_led);
}

static __exit void led_pwm_device_exit(void)
{
	resource_for_each_with_name("led-pwm", led_pwm_unregister_led);
}

device_initcall(led_pwm_device_init);
device_exitcall(led_pwm_device_exit);
