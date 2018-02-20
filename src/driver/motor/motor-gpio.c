/*
 * driver/motor/motor-gpio.c
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
#include <motor/motor.h>

struct motor_gpio_pdata_t {
	int a;
	int acfg;
	int b;
	int bcfg;
	int e;
	int ecfg;
	int speed;
};

static void motor_gpio_enable(struct motor_t * m)
{
	struct motor_gpio_pdata_t * pdat = (struct motor_gpio_pdata_t *)m->priv;
	if(pdat->e)
		gpio_set_value(pdat->e, 1);
}

static void motor_gpio_disable(struct motor_t * m)
{
	struct motor_gpio_pdata_t * pdat = (struct motor_gpio_pdata_t *)m->priv;
	if(pdat->e)
		gpio_set_value(pdat->e, 0);
}

static void motor_gpio_set(struct motor_t * m, int speed)
{
	struct motor_gpio_pdata_t * pdat = (struct motor_gpio_pdata_t *)m->priv;

	if(pdat->speed != speed)
	{
		if(speed < 0)
		{
			gpio_set_value(pdat->a, 0);
			gpio_set_value(pdat->b, 1);
		}
		else if(speed > 0)
		{
			gpio_set_value(pdat->b, 0);
			gpio_set_value(pdat->a, 1);
		}
		else
		{
			gpio_set_value(pdat->b, 0);
			gpio_set_value(pdat->a, 0);
		}
		pdat->speed = speed;
	}
}

static struct device_t * motor_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct motor_gpio_pdata_t * pdat;
	struct motor_t * m;
	struct device_t * dev;
	int a = dt_read_int(n, "a-gpio", -1);
	int b = dt_read_int(n, "b-gpio", -1);
	int e = dt_read_int(n, "enable-gpio", -1);

	if(!gpio_is_valid(a) || !gpio_is_valid(b))
		return NULL;

	pdat = malloc(sizeof(struct motor_gpio_pdata_t));
	if(!pdat)
		return NULL;

	m = malloc(sizeof(struct motor_t));
	if(!m)
	{
		free(pdat);
		return NULL;
	}

	pdat->a = a;
	pdat->acfg = dt_read_int(n, "a-gpio-config", -1);
	pdat->b = b;
	pdat->bcfg = dt_read_int(n, "b-gpio-config", -1);
	pdat->e = e;
	pdat->ecfg = dt_read_int(n, "enable-gpio-config", -1);
	pdat->speed = 0;

	m->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	m->enable = motor_gpio_enable;
	m->disable = motor_gpio_disable;
	m->set = motor_gpio_set;
	m->priv = pdat;

	if(pdat->a >= 0)
	{
		if(pdat->acfg >= 0)
			gpio_set_cfg(pdat->a, pdat->acfg);
		gpio_set_pull(pdat->a, GPIO_PULL_UP);
		gpio_set_direction(pdat->a, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->a, 0);
	}
	if(pdat->b >= 0)
	{
		if(pdat->bcfg >= 0)
			gpio_set_cfg(pdat->b, pdat->bcfg);
		gpio_set_pull(pdat->b, GPIO_PULL_UP);
		gpio_set_direction(pdat->b, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->a, 0);
	}
	if(pdat->e >= 0)
	{
		if(pdat->ecfg >= 0)
			gpio_set_cfg(pdat->e, pdat->ecfg);
		gpio_set_pull(pdat->e, GPIO_PULL_UP);
		gpio_set_direction(pdat->e, GPIO_DIRECTION_OUTPUT);
		gpio_set_value(pdat->e, 0);
	}

	if(!register_motor(&dev, m))
	{
		free_device_name(m->name);
		free(m->priv);
		free(m);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void motor_gpio_remove(struct device_t * dev)
{
	struct motor_t * m = (struct motor_t *)dev->priv;

	if(m && unregister_motor(m))
	{
		free_device_name(m->name);
		free(m->priv);
		free(m);
	}
}

static void motor_gpio_suspend(struct device_t * dev)
{
}

static void motor_gpio_resume(struct device_t * dev)
{
}

static struct driver_t motor_gpio = {
	.name		= "motor-gpio",
	.probe		= motor_gpio_probe,
	.remove		= motor_gpio_remove,
	.suspend	= motor_gpio_suspend,
	.resume		= motor_gpio_resume,
};

static __init void motor_gpio_driver_init(void)
{
	register_driver(&motor_gpio);
}

static __exit void motor_gpio_driver_exit(void)
{
	unregister_driver(&motor_gpio);
}

driver_initcall(motor_gpio_driver_init);
driver_exitcall(motor_gpio_driver_exit);
