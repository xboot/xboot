/*
 * driver/pwm-gpio.c
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
#include <pwm/pwm.h>

struct pwm_gpio_pdata_t
{
	struct timer_t timer;
	int gpio;
	int gpiocfg;
	int flag;
	int enable;
	int duty;
	int period;
	int polarity;
};

static void pwm_gpio_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_gpio_pdata_t * pdat = (struct pwm_gpio_pdata_t *)pwm->priv;
	pdat->duty = duty;
	pdat->period = period;
	pdat->polarity = polarity;
}

static void pwm_gpio_enable(struct pwm_t * pwm)
{
	struct pwm_gpio_pdata_t * pdat = (struct pwm_gpio_pdata_t *)pwm->priv;
	if(pdat->enable != 1)
	{
		pdat->enable = 1;
		timer_start_now(&pdat->timer, ms_to_ktime(1));
	}
}

static void pwm_gpio_disable(struct pwm_t * pwm)
{
	struct pwm_gpio_pdata_t * pdat = (struct pwm_gpio_pdata_t *)pwm->priv;
	pdat->enable = 0;
}

static int pwm_timer_function(struct timer_t * timer, void * data)
{
	struct pwm_t * pwm = (struct pwm_t *)(data);
	struct pwm_gpio_pdata_t * pdat = (struct pwm_gpio_pdata_t *)pwm->priv;

	if(pdat->enable)
	{
		pdat->flag = !pdat->flag;
		if(pdat->flag)
		{
			gpio_direction_output(pdat->gpio, pdat->polarity ? 0 : 1);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->duty));
		}
		else
		{
			gpio_direction_output(pdat->gpio, pdat->polarity ? 1 : 0);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->period - pdat->duty));
		}
		return 1;
	}

	pdat->flag = 0;
	gpio_direction_output(pdat->gpio, pdat->polarity ? 1 : 0);
	return 0;
}

static struct device_t * pwm_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_gpio_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	int gpio = dt_read_int(n, "gpio", -1);

	if(!gpio_is_valid(gpio))
		return NULL;

	pdat = malloc(sizeof(struct pwm_gpio_pdata_t));
	if(!pdat)
		return NULL;

	pwm = malloc(sizeof(struct pwm_t));
	if(!pwm)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, pwm_timer_function, pwm);
	pdat->gpio = gpio;
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->flag = 0;
	pdat->enable = 0;
	pdat->duty = 5 * 1000 * 1000;
	pdat->period = 10 * 1000 * 1000;
	pdat->polarity = 0;

	pwm->name = alloc_device_name(dt_read_name(n), -1);
	pwm->config = pwm_gpio_config;
	pwm->enable = pwm_gpio_enable;
	pwm->disable = pwm_gpio_disable;
	pwm->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
	gpio_set_pull(pdat->gpio, GPIO_PULL_UP);
	gpio_direction_output(pdat->gpio, pdat->polarity ? 1 : 0);

	if(!register_pwm(&dev, pwm))
	{
		timer_cancel(&pdat->timer);

		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void pwm_gpio_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_gpio_pdata_t * pdat = (struct pwm_gpio_pdata_t *)pwm->priv;

	if(pwm && unregister_pwm(pwm))
	{
		timer_cancel(&pdat->timer);

		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_gpio_suspend(struct device_t * dev)
{
}

static void pwm_gpio_resume(struct device_t * dev)
{
}

static struct driver_t pwm_gpio = {
	.name		= "pwm-gpio",
	.probe		= pwm_gpio_probe,
	.remove		= pwm_gpio_remove,
	.suspend	= pwm_gpio_suspend,
	.resume		= pwm_gpio_resume,
};

static __init void pwm_gpio_driver_init(void)
{
	register_driver(&pwm_gpio);
}

static __exit void pwm_gpio_driver_exit(void)
{
	unregister_driver(&pwm_gpio);
}

driver_initcall(pwm_gpio_driver_init);
driver_exitcall(pwm_gpio_driver_exit);
