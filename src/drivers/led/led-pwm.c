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

#include <xboot.h>
#include <pwm/pwm.h>
#include <led/led.h>

struct led_pwm_pdata_t {
	struct pwm_t * pwm;
	int period;
	int polarity;
	int brightness;
};

static void led_pwm_set(struct led_t * led, int brightness)
{
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;

	if(pdat->brightness != brightness)
	{
		if(brightness > 0)
		{
			int duty = brightness * pdat->period / (CONFIG_MAX_BRIGHTNESS + 1);
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

static struct device_t * led_pwm_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct led_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	struct led_t * led;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm", ""))))
		return NULL;

	pdat = malloc(sizeof(struct led_pwm_pdata_t));
	if(!pdat)
		return NULL;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return NULL;
	}

	pdat->pwm = pwm;
	pdat->period = dt_read_int(n, "period", 1000 * 1000);
	pdat->polarity = dt_read_bool(n, "polarity", 0);
	pdat->brightness = dt_read_int(n, "default-brightness", 0);

	led->name = dynamic_device_name(dt_read_name(n), dt_read_id(n));
	led->set = led_pwm_set,
	led->get = led_pwm_get,
	led->priv = pdat;

	if(pdat->brightness > 0)
	{
		int duty = pdat->brightness * pdat->period / (CONFIG_MAX_BRIGHTNESS + 1);
		pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
		pwm_enable(pdat->pwm);
	}
	else
	{
		pwm_disable(pdat->pwm);
	}

	if(!register_led(&dev, led))
	{
		free(led->priv);
		free(led->name);
		free(led);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void led_pwm_remove(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;

	if(led && unregister_led(led))
	{
		pdat->brightness = 0;
		pwm_disable(pdat->pwm);

		free(led->priv);
		free(led->name);
		free(led);
	}
}

static void led_pwm_suspend(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;

	pwm_disable(pdat->pwm);
}

static void led_pwm_resume(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_pdata_t * pdat = (struct led_pwm_pdata_t *)led->priv;

	if(pdat->brightness > 0)
	{
		int duty = pdat->brightness * pdat->period / (CONFIG_MAX_BRIGHTNESS + 1);
		pwm_config(pdat->pwm, duty, pdat->period, pdat->polarity);
		pwm_enable(pdat->pwm);
	}
	else
	{
		pwm_disable(pdat->pwm);
	}
}

struct driver_t led_pwm = {
	.name		= "led-pwm",
	.probe		= led_pwm_probe,
	.remove		= led_pwm_remove,
	.suspend	= led_pwm_suspend,
	.resume		= led_pwm_resume,
};

static __init void led_pwm_driver_init(void)
{
	register_driver(&led_pwm);
}

static __exit void led_pwm_driver_exit(void)
{
	unregister_driver(&led_pwm);
}

driver_initcall(led_pwm_driver_init);
driver_exitcall(led_pwm_driver_exit);
