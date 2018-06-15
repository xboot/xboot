/*
 * driver/proximity/proximity-gpio.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

	if(!register_proximity(&dev, p))
	{
		free_device_name(p->name);
		free(p->priv);
		free(p);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void proximity_gpio_remove(struct device_t * dev)
{
	struct proximity_t * p = (struct proximity_t *)dev->priv;

	if(p && unregister_proximity(p))
	{
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
