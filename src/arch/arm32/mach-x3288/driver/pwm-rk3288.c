/*
 * driver/pwm-rk3288.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <pwm/pwm.h>

#define PWM_CNT(x)			(((x) * 0x10) + 0x0)
#define PWM_PERIOD_HPR(x)	(((x) * 0x10) + 0x4)
#define PWM_DUTY_LPR(x)		(((x) * 0x10) + 0x8)
#define PWM_CTRL(x)			(((x) * 0x10) + 0xc)
#define PWM_INTSTS			(0x40)
#define PWM_INT_EN			(0x44)

struct pwm_rk3288_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int channel;
	int pwm;
	int pwmcfg;
};

static void pwm_rk3288_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_rk3288_pdata_t * pdat = (struct pwm_rk3288_pdata_t *)pwm->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	u32_t ctrl;

	if(pwm->__duty != duty)
	{
		write32(pdat->virt + PWM_DUTY_LPR(pdat->channel), (u32_t)(rate * duty / 1000000000L));
	}

	if(pwm->__period != period)
	{
		write32(pdat->virt + PWM_PERIOD_HPR(pdat->channel), (u32_t)(rate * period / 1000000000L));
	}

	if(pwm->__polarity != polarity)
	{
		ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
		if(polarity)
			ctrl &= ~(0x3 << 3);
		else
			ctrl |= (0x3 << 3);
		write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
	}
}

static void pwm_rk3288_enable(struct pwm_t * pwm)
{
	struct pwm_rk3288_pdata_t * pdat = (struct pwm_rk3288_pdata_t *)pwm->priv;
	u32_t ctrl;

	if((pdat->pwm >= 0) && (pdat->pwmcfg >= 0))
		gpio_set_cfg(pdat->pwm, pdat->pwmcfg);
	clk_enable(pdat->clk);

	ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
	ctrl &= ~((0x1 << 8) | (0x1 << 5) | (0x3 << 1));
	ctrl |= ((0x0 << 8) | (0x0 << 5) | (0x1 << 1));
	ctrl |= (0x1 << 0);
	write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
}

static void pwm_rk3288_disable(struct pwm_t * pwm)
{
	struct pwm_rk3288_pdata_t * pdat = (struct pwm_rk3288_pdata_t *)pwm->priv;
	u32_t ctrl;

	ctrl = read32(pdat->virt + PWM_CTRL(pdat->channel));
	ctrl &= ~(0x1 << 0);
	write32(pdat->virt + PWM_CTRL(pdat->channel), ctrl);
	clk_disable(pdat->clk);
}

static struct device_t * pwm_rk3288_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_rk3288_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 3)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct pwm_rk3288_pdata_t));
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

	pwm->name = alloc_device_name(dt_read_name(n), -1);
	pwm->config = pwm_rk3288_config;
	pwm->enable = pwm_rk3288_enable;
	pwm->disable = pwm_rk3288_disable;
	pwm->priv = pdat;

	write32(pdat->virt + PWM_INT_EN, (read32(pdat->virt + PWM_INT_EN) & ~(0x1 << pdat->channel)));
	write32(pdat->virt + PWM_CTRL(pdat->channel), (read32(pdat->virt + PWM_CTRL(pdat->channel)) & ~(0x1 << 0)));

	if(!register_pwm(&dev, pwm))
	{
		free(pdat->clk);

		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void pwm_rk3288_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_rk3288_pdata_t * pdat = (struct pwm_rk3288_pdata_t *)pwm->priv;

	if(pwm && unregister_pwm(pwm))
	{
		free(pdat->clk);

		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_rk3288_suspend(struct device_t * dev)
{
}

static void pwm_rk3288_resume(struct device_t * dev)
{
}

static struct driver_t pwm_rk3288 = {
	.name		= "pwm-rk3288",
	.probe		= pwm_rk3288_probe,
	.remove		= pwm_rk3288_remove,
	.suspend	= pwm_rk3288_suspend,
	.resume		= pwm_rk3288_resume,
};

static __init void pwm_rk3288_driver_init(void)
{
	register_driver(&pwm_rk3288);
}

static __exit void pwm_rk3288_driver_exit(void)
{
	unregister_driver(&pwm_rk3288);
}

driver_initcall(pwm_rk3288_driver_init);
driver_exitcall(pwm_rk3288_driver_exit);
