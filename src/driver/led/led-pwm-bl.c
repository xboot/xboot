/*
 * driver/led/led-pwm-bl.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
		if(brightness <= 0)
			regulator_disable(pdat->regulator);
		else if(pdat->brightness <= 0)
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
	pdat->brightness = -1;

	led->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	led->set = led_pwm_bl_set;
	led->get = led_pwm_bl_get;
	led->priv = pdat;

	led_pwm_bl_set(led, dt_read_int(n, "default-brightness", 0));

	if(!register_led(&dev, led))
	{
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
		if(pdat->regulator)
			free(pdat->regulator);

		free_device_name(led->name);
		free(led->priv);
		free(led);
	}
}

static void led_pwm_bl_suspend(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;
	if(pdat->brightness > 0)
		regulator_disable(pdat->regulator);
	led_pwm_bl_set_brightness(pdat, 0);
}

static void led_pwm_bl_resume(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_pwm_bl_pdata_t * pdat = (struct led_pwm_bl_pdata_t *)led->priv;
	if(pdat->brightness > 0)
		regulator_enable(pdat->regulator);
	led_pwm_bl_set_brightness(pdat, pdat->brightness);
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
