/*
 * driver/proximity/proximity-gpio.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <proximity/proximity.h>

struct proximity_gpio_pdata_t {
	int gpio;
	int gpiocfg;
	int active_low;
};

static int proximity_gpio_get(struct proximity_t * p)
{
	struct proximity_gpio_pdata_t * pdat = (struct proximity_gpio_pdata_t *)p->priv;
	if(pdat->active_low)
		return gpio_get_value(pdat->gpio) ? 1000000 : 0;
	return gpio_get_value(pdat->gpio) ? 0 : 1000000;
}

static struct device_t * proximity_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct proximity_gpio_pdata_t * pdat;
	struct proximity_t * p;
	struct device_t * dev;

	if(!gpio_is_valid(dt_read_int(n, "gpio", -1)))
		return NULL;

	pdat = malloc(sizeof(struct proximity_gpio_pdata_t));
	if(!pdat)
		return NULL;

	p = malloc(sizeof(struct proximity_t));
	if(!p)
	{
		free(pdat);
		return NULL;
	}

	pdat->gpio = dt_read_int(n, "gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->active_low = dt_read_bool(n, "active-low", 0);

	p->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	p->get = proximity_gpio_get;
	p->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_set_direction(pdat->gpio, GPIO_DIRECTION_OUTPUT);

	if(!(dev = register_proximity(p, drv)))
	{
		free_device_name(p->name);
		free(p->priv);
		free(p);
		return NULL;
	}
	return dev;
}

static void proximity_gpio_remove(struct device_t * dev)
{
	struct proximity_t * p = (struct proximity_t *)dev->priv;

	if(p)
	{
		unregister_proximity(p);
		free_device_name(p->name);
		free(p->priv);
		free(p);
	}
}

static void proximity_gpio_suspend(struct device_t * dev)
{
}

static void proximity_gpio_resume(struct device_t * dev)
{
}

static struct driver_t proximity_gpio = {
	.name		= "proximity-gpio",
	.probe		= proximity_gpio_probe,
	.remove		= proximity_gpio_remove,
	.suspend	= proximity_gpio_suspend,
	.resume		= proximity_gpio_resume,
};

static __init void proximity_gpio_driver_init(void)
{
	register_driver(&proximity_gpio);
}

static __exit void proximity_gpio_driver_exit(void)
{
	unregister_driver(&proximity_gpio);
}

driver_initcall(proximity_gpio_driver_init);
driver_exitcall(proximity_gpio_driver_exit);
