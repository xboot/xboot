/*
 * driver/led/led-pwm-bl.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <gpio/gpio.h>
#include <pwm/pwm.h>
#include <regulator/regulator.h>
#include <led/led.h>

struct led_pwm_bl_pdata_t {
	struct pwm_t * pwm;
	char * regulator;
	int period;
	int polarity;
	int from;
	int to;
	int brightness;
};

static void led_pwm_bl_set_brightness(struct led_pwm_bl_pdata_t * pdat, int brightness)
{
	if(brightness > 0)
	{
		int duty = pdat->from + (pdat->to - pdat->from) * brightness / CONFIG_MAX_BRIGHTNESS;
		pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
		pwm_enable(pdat->pwm);
	}
	else
	{
		pwm_disable(pdat->pwm);
	}
}

static void led_pwm_bl_set(struct led_t * led, int brightness)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;

	if(pdat->brightness != brightness)
	{
		if(brightness == 0)
			regulator_disable(pdat->regulator);
		else if(pdat->brightness == 0)
			regulator_enable(pdat->regulator);
		led_pwm_bl_set_brightness(pdat, brightness);
		pdat->brightness = brightness;
	}
}

static int led_pwm_bl_get(struct led_t * led)
{
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;
	return pdat->brightness;
}

static struct device_t * led_pwm_bl_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct led_pwm_bl_pdata_t * pdat;
	struct pwm_t * pwm;
	struct led_t * led;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct led_pwm_bl_pdata_t));
	if(!pdat)
		return NULL;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return NULL;
	}

	pdat->pwm = pwm;
	pdat->regulator = strdup(dt_read_string(n, "regulator-name", NULL));
	pdat->period = dt_read_int(n, "pwm-period-ns", 1000 * 1000);
	pdat->polarity = dt_read_bool(n, "pwm-polarity", 0);
	pdat->from = dt_read_int(n, "pwm-percent-from", 0) * pdat->period / 100;
	pdat->to = dt_read_int(n, "pwm-percent-to", 100) * pdat->period / 100;
	pdat->brightness = dt_read_int(n, "default-brightness", 0);

	led->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	led->set = led_pwm_bl_set,
	led->get = led_pwm_bl_get,
	led->priv = pdat;

	if(pdat->brightness > 0)
		regulator_enable(pdat->regulator);
	else
		regulator_disable(pdat->regulator);
	led_pwm_bl_set_brightness(pdat, pdat->brightness);

	if(!register_led(&dev, led))
	{
		regulator_disable(pdat->regulator);
		led_pwm_bl_set_brightness(pdat, 0);
		if(pdat->regulator)
			free(pdat->regulator);

		free_device_name(led->name);
		free(led->priv);
		free(led);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void led_pwm_bl_remove(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;

	if(led && unregister_led(led))
	{
		regulator_disable(pdat->regulator);
		led_pwm_bl_set_brightness(pdat, 0);
		if(pdat->regulator)
			free(pdat->regulator);

		free_device_name(led->name);
		free(led->priv);
		free(led);
	}
}

static void led_pwm_bl_suspend(struct device_t * dev)
{
}

static void led_pwm_bl_resume(struct device_t * dev)
{
}

static struct driver_t led_pwm_bl = {
	.name		= "led-pwm-bl",
	.probe		= led_pwm_bl_probe,
	.remove		= led_pwm_bl_remove,
	.suspend	= led_pwm_bl_suspend,
	.resume		= led_pwm_bl_resume,
};

static __init void led_pwm_bl_driver_init(void)
{
	register_driver(&led_pwm_bl);
}

static __exit void led_pwm_bl_driver_exit(void)
{
	unregister_driver(&led_pwm_bl);
}

driver_initcall(led_pwm_bl_driver_init);
driver_exitcall(led_pwm_bl_driver_exit);
