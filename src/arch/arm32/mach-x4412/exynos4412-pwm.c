/*
 * exynos4412-pwm.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-timer.h>

struct exynos4412_pwm_data_t
{
	const char * name;
	int gpio;
	physical_addr_t regbase;

	u64_t __clk;
};

static struct exynos4412_pwm_data_t pwm_datas[] = {
	{
		.name		= "pwm0",
		.gpio		= EXYNOS4412_GPD0(0),
		.regbase	= EXYNOS4412_TIMER0_BASE,
	}, {
		.name		= "pwm1",
		.gpio		= EXYNOS4412_GPD0(1),
		.regbase	= EXYNOS4412_TIMER1_BASE,
	},
	{
		.name		= "pwm2",
		.gpio		= EXYNOS4412_GPD0(2),
		.regbase	= EXYNOS4412_TIMER2_BASE,
	},
	{
		.name		= "pwm3",
		.gpio		= EXYNOS4412_GPD0(3),
		.regbase	= EXYNOS4412_TIMER3_BASE,
	},
};

static void exynos4412_pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period)
{
	struct exynos4412_pwm_data_t * dat = (struct exynos4412_pwm_data_t *)pwm->priv;
	u32_t val;

	if(pwm->__duty != duty)
	{
		val = dat->__clk * duty / 1000000000L;
		writel(dat->regbase + EXYNOS4412_TCMPB, val);
	}

	if(pwm->__period != period)
	{
		val = dat->__clk * period / 1000000000L;
		writel(dat->regbase + EXYNOS4412_TCNTB, val);
	}

	writel(EXYNOS4412_TCON, (readl(EXYNOS4412_TCON) & ~(0x1 << 1)) | (0x1 << 1));
	writel(EXYNOS4412_TCON, (readl(EXYNOS4412_TCON) & ~(0x1 << 1)) | (0x0 << 1));
}

static void exynos4412_pwm_start(struct pwm_t * pwm, u32_t duty, u32_t period)
{
	struct exynos4412_pwm_data_t * dat = (struct exynos4412_pwm_data_t *)pwm->priv;
	u32_t val;

	if(pwm->__enable == TRUE)
		return;

	gpio_set_cfg(dat->gpio, 0x2);
	gpio_set_pull(dat->gpio, GPIO_PULL_UP);

	val = readl(EXYNOS4412_TCON);
	if(strcmp(dat->name, "pwm0") == 0)
	{
		val &= ~(0xf << 0);
		val |= (0x9 << 0);

		writel(EXYNOS4412_TCFG1, (readl(EXYNOS4412_TCFG1) & ~(0xf<<0)) | (0x0<<0));
		clk_enable("DIV-PRESCALER0");
		dat->__clk = clk_get_rate("DIV-PRESCALER0");
	}
	else if(strcmp(dat->name, "pwm1") == 0)
	{
		val &= ~(0xf << 8);
		val |= (0x9 << 8);

		writel(EXYNOS4412_TCFG1, (readl(EXYNOS4412_TCFG1) & ~(0xf<<4)) | (0x0<<4));
		clk_enable("DIV-PRESCALER0");
		dat->__clk = clk_get_rate("DIV-PRESCALER0");
	}
	else if(strcmp(dat->name, "pwm2") == 0)
	{
		val &= ~(0xf << 12);
		val |= (0x9 << 12);

		writel(EXYNOS4412_TCFG1, (readl(EXYNOS4412_TCFG1) & ~(0xf<<8)) | (0x0<<8));
		clk_enable("DIV-PRESCALER1");
		dat->__clk = clk_get_rate("DIV-PRESCALER1");
	}
	else if(strcmp(dat->name, "pwm3") == 0)
	{
		val &= ~(0xf << 16);
		val |= (0x9 << 16);

		writel(EXYNOS4412_TCFG1, (readl(EXYNOS4412_TCFG1) & ~(0xf<<12)) | (0x0<<12));
		clk_enable("DIV-PRESCALER1");
		dat->__clk = clk_get_rate("DIV-PRESCALER1");
	}

	exynos4412_pwm_config(pwm, duty, period);
	writel(EXYNOS4412_TCON, val);
}

static void exynos4412_pwm_stop(struct pwm_t * pwm)
{
	struct exynos4412_pwm_data_t * dat = (struct exynos4412_pwm_data_t *)pwm->priv;
	u32_t val;

	if(pwm->__enable == FALSE)
		return;

	val = readl(EXYNOS4412_TCON);
	if(strcmp(dat->name, "pwm0") == 0)
	{
		val &= ~(0x1 << 0);
		clk_disable("DIV-PRESCALER0");
	}
	else if(strcmp(dat->name, "pwm1") == 0)
	{
		val &= ~(0x1 << 8);
		clk_disable("DIV-PRESCALER0");
	}
	else if(strcmp(dat->name, "pwm2") == 0)
	{
		val &= ~(0x1 << 12);
		clk_disable("DIV-PRESCALER1");
	}
	else if(strcmp(dat->name, "pwm3") == 0)
	{
		val &= ~(0x1 << 16);
		clk_disable("DIV-PRESCALER1");
	}
	writel(EXYNOS4412_TCON, val);
}

static __init void exynos4412_pwm_init(void)
{
	struct pwm_t * pwm;
	int i;

	for(i = 0; i < ARRAY_SIZE(pwm_datas); i++)
	{
		pwm = malloc(sizeof(struct pwm_t));
		if(!pwm)
			continue;

		pwm->name = pwm_datas[i].name;
		pwm->start = exynos4412_pwm_start;
		pwm->config = exynos4412_pwm_config;
		pwm->stop = exynos4412_pwm_stop;
		pwm->priv = &pwm_datas[i];

		if(register_pwm(pwm))
			LOG("Register pwm '%s'", pwm->name);
		else
			LOG("Failed to register pwm '%s'", pwm->name);
	}
}

static __exit void exynos4412_pwm_exit(void)
{
	struct pwm_t * pwm;
	int i;

	for(i = 0; i < ARRAY_SIZE(pwm_datas); i++)
	{
		pwm = search_pwm(pwm_datas[i].name);
		if(!pwm)
			continue;
		if(unregister_pwm(pwm))
			LOG("Unregister pwm '%s'", pwm->name);
		else
			LOG("Failed to unregister pwm '%s'", pwm->name);
		free(pwm);
	}
}

core_initcall(exynos4412_pwm_init);
core_exitcall(exynos4412_pwm_exit);
