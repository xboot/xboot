/*
 * driver/pwm-rk3128.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <pwm/pwm.h>

#define PWM_CNT(x)			(((x) * 0x10) + 0x0)
#define PWM_PERIOD_HPR(x)	(((x) * 0x10) + 0x4)
#define PWM_DUTY_LPR(x)		(((x) * 0x10) + 0x8)
#define PWM_CTRL(x)			(((x) * 0x10) + 0xc)
#define PWM_INTSTS			(0x40)
#define PWM_INT_EN			(0x44)

struct pwm_rk3128_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int channel;
	int pwm;
	int pwmcfg;

	int enable;
	int duty;
	int period;
	int polarity;
};

static void pwm_rk3128_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_rk3128_pdata_t * pdat = (struct pwm_rk3128_pdata_t *)pwm->priv;
	u64_t rate;
	u32_t ctrl;

	if(pdat->duty != duty || pdat->period != period)
	{
		rate = clk_get_rate(pdat->clk);
		if(pdat->duty != duty)
		{
			pdat->duty = duty;
			write32(pdat->virt + PWM_DUTY_LPR(pdat->channel), (u32_t)(rate * duty / 1000000000L));
		}
		if(pdat->period != period)
		{
			pdat->period = period;
			write32(pdat->virt + PWM_PERIOD_HPR(pdat->channel), (u32_t)(rate * period / 1000000000L));
		}
	}
	if(pdat->polarity != polarity)
	{
		pdat->polarity = polarity;
		ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
		if(polarity)
			ctrl &= ~(0x3 << 3);
		else
			ctrl |= (0x3 << 3);
		write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
	}
}

static void pwm_rk3128_enable(struct pwm_t * pwm)
{
	struct pwm_rk3128_pdata_t * pdat = (struct pwm_rk3128_pdata_t *)pwm->priv;
	u32_t ctrl;

	if(pdat->enable != 1)
	{
		pdat->enable = 1;
		if((pdat->pwm >= 0) && (pdat->pwmcfg >= 0))
			gpio_set_cfg(pdat->pwm, pdat->pwmcfg);
		clk_enable(pdat->clk);
		ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
		ctrl &= ~((0x1 << 8) | (0x1 << 5) | (0x3 << 1));
		ctrl |= ((0x0 << 8) | (0x0 << 5) | (0x1 << 1));
		ctrl |= (0x1 << 0);
		write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
	}
}

static void pwm_rk3128_disable(struct pwm_t * pwm)
{
	struct pwm_rk3128_pdata_t * pdat = (struct pwm_rk3128_pdata_t *)pwm->priv;
	u32_t ctrl;

	if(pdat->enable != 0)
	{
		pdat->enable = 0;
		ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
		ctrl &= ~(0x1 << 0);
		write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
		clk_disable(pdat->clk);
	}
}

static struct device_t * pwm_rk3128_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_rk3128_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 3)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct pwm_rk3128_pdata_t));
	if(!pdat)
		return NULL;

	pwm = malloc(sizeof(struct pwm_t));
	if(!pwm)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->channel = channel;
	pdat->pwm = dt_read_int(n, "pwm-gpio", -1);
	pdat->pwmcfg = dt_read_int(n, "pwm-gpio-config", -1);
	pdat->enable = -1;
	pdat->duty = 500 * 1000;
	pdat->period = 1000 * 1000;
	pdat->polarity = 0;

	pwm->name = alloc_device_name(dt_read_name(n), -1);
	pwm->config = pwm_rk3128_config;
	pwm->enable = pwm_rk3128_enable;
	pwm->disable = pwm_rk3128_disable;
	pwm->priv = pdat;

	write32(pdat->virt + PWM_INT_EN, (read32(pdat->virt + PWM_INT_EN) & ~(0x1 << pdat->channel)));
	write32(pdat->virt + PWM_CTRL(pdat->channel), (read32(pdat->virt + PWM_CTRL(pdat->channel)) | (0x3 << 3)));
	write32(pdat->virt + PWM_CTRL(pdat->channel), (read32(pdat->virt + PWM_CTRL(pdat->channel)) & ~(0x1 << 0)));

	if(!(dev = register_pwm(pwm, drv)))
	{
		free(pdat->clk);
		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
		return NULL;
	}
	return dev;
}

static void pwm_rk3128_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_rk3128_pdata_t * pdat = (struct pwm_rk3128_pdata_t *)pwm->priv;

	if(pwm)
	{
		unregister_pwm(pwm);
		free(pdat->clk);
		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_rk3128_suspend(struct device_t * dev)
{
}

static void pwm_rk3128_resume(struct device_t * dev)
{
}

static struct driver_t pwm_rk3128 = {
	.name		= "pwm-rk3128",
	.probe		= pwm_rk3128_probe,
	.remove		= pwm_rk3128_remove,
	.suspend	= pwm_rk3128_suspend,
	.resume		= pwm_rk3128_resume,
};

static __init void pwm_rk3128_driver_init(void)
{
	register_driver(&pwm_rk3128);
}

static __exit void pwm_rk3128_driver_exit(void)
{
	unregister_driver(&pwm_rk3128);
}

driver_initcall(pwm_rk3128_driver_init);
driver_exitcall(pwm_rk3128_driver_exit);
