/*
 * bcm2836-pwm.c
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

#include <pwm/pwm.h>
#include <bcm2836-gpio.h>
#include <bcm2836/reg-pwm.h>

struct bcm2836_pwm_data_t
{
	const char * name;
	char * clk;
	int ch;
	int pin;
	int pincfg;
	physical_addr_t phys;
};

struct bcm2836_pwm_pdata_t
{
	const char * name;
	char * clk;
	int ch;
	int pin;
	int pincfg;
	virtual_addr_t virt;
	unsigned long scaler;
};

static struct bcm2836_pwm_data_t datas[] = {
	{
		.name	= "pwm0",
		.clk	= "pwm-clk",
		.ch		= 0,
		.pin	= BCM2836_GPIO(12),
		.pincfg	= 0x0,
		.phys	= BCM2836_PWM_BASE,
	}, {
		.name	= "pwm1",
		.clk	= "pwm-clk",
		.ch		= 1,
		.pin	= BCM2836_GPIO(13),
		.pincfg	= 0x0,
		.phys	= BCM2836_PWM_BASE,
	}
};

static void bcm2836_pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period, bool_t polarity)
{
	struct bcm2836_pwm_pdata_t * pdat = (struct bcm2836_pwm_pdata_t *)pwm->priv;
	u32_t ctrl;

	if(pwm->__duty != duty)
	{
		write32(pdat->virt + PWM_DUTY(pdat->ch), duty / pdat->scaler);
	}

	if(pwm->__period != period)
	{
		write32(pdat->virt + PWM_PERIOD(pdat->ch), period / pdat->scaler);
	}

	if(pwm->__polarity != polarity)
	{
		ctrl = read32(pdat->virt + PWM_CTRL);
		if(polarity)
			ctrl |= (0x10 << PWM_CTRL_SHIFT(pdat->ch));
		else
			ctrl &= ~(0x10 << PWM_CTRL_SHIFT(pdat->ch));
		write32(pdat->virt + PWM_CTRL, ctrl);
	}
}

static void bcm2836_pwm_enable(struct pwm_t * pwm)
{
	struct bcm2836_pwm_pdata_t * pdat = (struct bcm2836_pwm_pdata_t *)pwm->priv;
	u32_t ctrl;

	gpio_set_cfg(pdat->pin, pdat->pincfg);
	clk_enable(pdat->clk);
	pdat->scaler = 1000000000ULL / clk_get_rate(pdat->clk);

	ctrl = read32(pdat->virt + PWM_CTRL);
	ctrl &= ~(PWM_CTRL_MASK << PWM_CTRL_SHIFT(pdat->ch));
	ctrl |= (0x81 << PWM_CTRL_SHIFT(pdat->ch));
	write32(pdat->virt + PWM_CTRL, ctrl);
}

static void bcm2836_pwm_disable(struct pwm_t * pwm)
{
	struct bcm2836_pwm_pdata_t * pdat = (struct bcm2836_pwm_pdata_t *)pwm->priv;
	u32_t ctrl;

	ctrl = read32(pdat->virt + PWM_CTRL);
	ctrl &= ~(PWM_CTRL_MASK << PWM_CTRL_SHIFT(pdat->ch));
	ctrl |= (0x00 << PWM_CTRL_SHIFT(pdat->ch));
	write32(pdat->virt + PWM_CTRL, ctrl);

	clk_disable(pdat->clk);
}

static __init void bcm2836_pwm_init(void)
{
	struct bcm2836_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	int i;

	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct bcm2836_pwm_pdata_t));
		if(!pdat)
			continue;

		pwm = malloc(sizeof(struct pwm_t));
		if(!pwm)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->clk = datas[i].clk;
		pdat->ch = datas[i].ch;
		pdat->pin = datas[i].pin;
		pdat->pincfg = datas[i].pincfg;
		pdat->virt = phys_to_virt(datas[i].phys);

		pwm->name = pdat->name;
		pwm->config = bcm2836_pwm_config;
		pwm->enable = bcm2836_pwm_enable;
		pwm->disable = bcm2836_pwm_disable;
		pwm->priv = pdat;

		register_pwm(pwm);
	}
}
core_initcall(bcm2836_pwm_init);
