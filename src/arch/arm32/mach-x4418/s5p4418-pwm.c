/*
 * s5p4418-pwm.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <s5p4418/reg-gpio.h>
#include <s5p4418/reg-timer.h>

#define TCON_START(ch)				(0x1 << (4 * ch + 0))
#define TCON_MANUALUPDATE(ch)		(0x1 << (4 * ch + 1))
#define TCON_INVERT(ch)				(0x1 << (4 * ch + 2))
#define TCON_AUTORELOAD(ch)			(0x1 << (4 * ch + 3))

struct s5p4418_pwm_data_t
{
	const char * name;
	int id;
	int gpio;
	physical_addr_t regbase;
};

static struct s5p4418_pwm_data_t pwm_datas[] = {
	{
		.name		= "pwm0",
		.id			= 0,
		.gpio		= S5P4418_GPIOD(1),
		.regbase	= S5P4418_TIMER0_BASE,
	}, {
		.name		= "pwm1",
		.id			= 1,
		.gpio		= S5P4418_GPIOC(13),
		.regbase	= S5P4418_TIMER1_BASE,
	},
	{
		.name		= "pwm2",
		.id			= 2,
		.gpio		= S5P4418_GPIOC(14),
		.regbase	= S5P4418_TIMER2_BASE,
	},
	{
		.name		= "pwm3",
		.id			= 3,
		.gpio		= S5P4418_GPIOD(0),
		.regbase	= S5P4418_TIMER3_BASE,
	},
};

static u64_t s5p4418_pwm_calc_tin(struct pwm_t * pwm, u32_t period)
{
	struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
	u64_t rate, freq = 1000000000L / period;
	u8_t div, shift;

	if(dat->id < 2)
		rate = clk_get_rate("DIV-PRESCALER0");
	else
		rate = clk_get_rate("DIV-PRESCALER1");

	for(div = 0; div < 4; div++)
	{
		if((rate >> div) < freq)
			break;
	}

	shift = dat->id * 4;
	write32(S5P4418_TCFG1, (read32(S5P4418_TCFG1) & ~(0xf<<shift)) | (div<<shift));

	return rate >> div;
}

static void s5p4418_pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period, bool_t polarity)
{
	struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
	u64_t rate;
	u32_t tcnt, tcmp;
	u32_t tcon;

	if((pwm->__duty != duty) || (pwm->__period != period))
	{
		rate = s5p4418_pwm_calc_tin(pwm, period);

		if(pwm->__duty != duty)
		{
			tcmp = rate * duty / 1000000000L;
			write32(dat->regbase + S5P4418_TCMPB, tcmp);
		}

		if(pwm->__period != period)
		{
			tcnt = rate * period / 1000000000L;
			write32(dat->regbase + S5P4418_TCNTB, tcnt);
		}

		tcon = read32(S5P4418_TCON);
		tcon |= TCON_MANUALUPDATE(dat->id);
		write32(S5P4418_TCON, tcon);

		tcon &= ~TCON_MANUALUPDATE(dat->id);
		write32(S5P4418_TCON, tcon);
	}

	if(pwm->__polarity != polarity)
	{
		tcon = read32(S5P4418_TCON);
		if(polarity)
			tcon |= TCON_INVERT(dat->id);
		else
			tcon &= ~TCON_INVERT(dat->id);
		write32(S5P4418_TCON, tcon);
	}
}

static void s5p4418_pwm_enable(struct pwm_t * pwm)
{
	struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
	u32_t tcon;

	if(dat->id == 0)
		gpio_set_cfg(dat->gpio, 0x1);
	else
		gpio_set_cfg(dat->gpio, 0x2);
	gpio_set_pull(dat->gpio, GPIO_PULL_UP);

	if(dat->id < 2)
		clk_enable("DIV-PRESCALER0");
	else
		clk_enable("DIV-PRESCALER1");

	tcon = read32(S5P4418_TCON);
	tcon &= ~TCON_START(dat->id);
	tcon |= TCON_MANUALUPDATE(dat->id);
	write32(S5P4418_TCON, tcon);

	tcon &= ~TCON_MANUALUPDATE(dat->id);
	tcon |= TCON_START(dat->id) | TCON_AUTORELOAD(dat->id);
	write32(S5P4418_TCON, tcon);
}

static void s5p4418_pwm_disable(struct pwm_t * pwm)
{
	struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
	u32_t tcon;

	tcon = read32(S5P4418_TCON);
	tcon &= ~TCON_AUTORELOAD(dat->id);
	write32(S5P4418_TCON, tcon);

	if(dat->id < 2)
		clk_disable("DIV-PRESCALER0");
	else
		clk_disable("DIV-PRESCALER1");
}

static __init void s5p4418_pwm_init(void)
{
	struct pwm_t * pwm;
	int i;

	for(i = 0; i < ARRAY_SIZE(pwm_datas); i++)
	{
		pwm = malloc(sizeof(struct pwm_t));
		if(!pwm)
			continue;

		pwm->name = pwm_datas[i].name;
		pwm->config = s5p4418_pwm_config;
		pwm->enable = s5p4418_pwm_enable;
		pwm->disable = s5p4418_pwm_disable;
		pwm->priv = &pwm_datas[i];

		if(register_pwm(pwm))
			LOG("Register pwm '%s'", pwm->name);
		else
			LOG("Failed to register pwm '%s'", pwm->name);
	}
}

static __exit void s5p4418_pwm_exit(void)
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

core_initcall(s5p4418_pwm_init);
core_exitcall(s5p4418_pwm_exit);
