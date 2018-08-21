/*
 * driver/pwm-gpio.c
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
			gpio_set_value(pdat->gpio, pdat->polarity ? 0 : 1);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->duty));
		}
		else
		{
			gpio_set_value(pdat->gpio, pdat->polarity ? 1 : 0);
			timer_forward_now(&pdat->timer, ns_to_ktime(pdat->period - pdat->duty));
		}
		return 1;
	}

	pdat->flag = 0;
	gpio_set_value(pdat->gpio, pdat->polarity ? 1 : 0);
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
	gpio_set_direction(pdat->gpio, GPIO_DIRECTION_OUTPUT);
	gpio_set_value(pdat->gpio, pdat->polarity ? 1 : 0);

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
