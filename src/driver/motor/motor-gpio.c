/*
 * driver/motor/motor-gpio.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <motor/motor.h>

struct motor_gpio_pdata_t {
	struct timer_t timer;
	int a;
	int acfg;
	int b;
	int bcfg;
	int e;
	int ecfg;
	int duty;
	int period;
	int maxspeed;
	int speed;
	int flag;
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
	int restart = 0;

	if(pdat->speed != speed)
	{
		if(pdat->speed == 0)
			restart = 1;
		if(speed < -pdat->maxspeed)
			speed = -pdat->maxspeed;
		else if(speed > pdat->maxspeed)
			speed = pdat->maxspeed;
		pdat->duty = (s64_t)pdat->period * abs(speed) / pdat->maxspeed;
		pdat->speed = speed;
		if(restart != 0)
			timer_start_now(&pdat->timer, ns_to_ktime(pdat->duty));
	}
}

static int motor_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct motor_t * m = (struct motor_t *)(data);
	struct motor_gpio_pdata_t * pdat = (struct motor_gpio_pdata_t *)m->priv;

	if(pdat->speed < 0)
	{
		pdat->flag = !pdat->flag;
		if(pdat->flag)
		{
			gpio_set_value(pdat->a, 0);
			gpio_set_value(pdat->b, 1);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->duty));
		}
		else
		{
			gpio_set_value(pdat->a, 0);
			gpio_set_value(pdat->b, 0);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->period - pdat->duty));
		}
		return 1;
	}
	else if(pdat->speed > 0)
	{
		pdat->flag = !pdat->flag;
		if(pdat->flag)
		{
			gpio_set_value(pdat->b, 0);
			gpio_set_value(pdat->a, 1);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->duty));
		}
		else
		{
			gpio_set_value(pdat->b, 0);
			gpio_set_value(pdat->a, 0);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->period - pdat->duty));
		}
		return 1;
	}

	pdat->flag = 0;
	gpio_set_value(pdat->a, 0);
	gpio_set_value(pdat->b, 0);
	return 0;
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

	timer_init(&pdat->timer, motor_gpio_timer_function, m);
	pdat->a = a;
	pdat->acfg = dt_read_int(n, "a-gpio-config", -1);
	pdat->b = b;
	pdat->bcfg = dt_read_int(n, "b-gpio-config", -1);
	pdat->e = e;
	pdat->ecfg = dt_read_int(n, "enable-gpio-config", -1);
	pdat->period = dt_read_int(n, "period-ns", 10 * 1000 * 1000);
	pdat->duty = pdat->period / 2;
	pdat->maxspeed = abs(dt_read_int(n, "max-speed-rpm", 1000));
	pdat->speed = 0;
	pdat->flag = 0;

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
		gpio_set_value(pdat->b, 0);
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
		timer_cancel(&pdat->timer);

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
	struct motor_gpio_pdata_t * pdat = (struct motor_gpio_pdata_t *)m->priv;

	if(m && unregister_motor(m))
	{
		timer_cancel(&pdat->timer);

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
