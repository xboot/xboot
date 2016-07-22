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

static struct device_t * led_gpio_probe(struct driver_t * drv, void * dt)
{
	struct led_gpio_pdata_t * pdat;
	struct led_t * led;
	struct device_t * dev;
	char name[64];

	if(!gpio_is_valid(dt_read_int(dt, "gpio", -1)))
		return NULL;

	pdat = malloc(sizeof(struct led_gpio_pdata_t));
	if(!pdat)
		return NULL;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		return NULL;
	}

	pdat->gpio = dt_read_int(dt, "gpio", -1);
	pdat->active_low = dt_read_bool(dt, "active-low", 0);
	pdat->brightness = dt_read_int(dt, "default-brightness", 0);

	snprintf(name, sizeof(name), "%s.%d", drv->name, 0);
	led->name = strdup(name);
	led->set = led_gpio_set,
	led->get = led_gpio_get,
	led->priv = pdat;

	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	if(pdat->brightness > 0)
		gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
	else
		gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);

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

static void led_gpio_remove(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	if(led && unregister_led(led))
	{
		pdat->brightness = 0;
		gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);

		free(led->priv);
		free(led->name);
		free(led);
	}
}

static void led_gpio_suspend(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void led_gpio_resume(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_gpio_pdata_t * pdat = (struct led_gpio_pdata_t *)led->priv;

	if(pdat->brightness > 0)
		gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
	else
		gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

struct driver_t led_gpio = {
	.name		= "led-gpio",
	.probe		= led_gpio_probe,
	.remove		= led_gpio_remove,
	.suspend	= led_gpio_suspend,
	.resume		= led_gpio_resume,
};

static __init void led_gpio_driver_init(void)
{
	register_driver(&led_gpio);
}

static __exit void led_gpio_driver_exit(void)
{
	unregister_driver(&led_gpio);
}

driver_initcall(led_gpio_driver_init);
driver_exitcall(led_gpio_driver_exit);
