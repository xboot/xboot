/*
 * s5p4418-pwm.c
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
#include <s5p4418-rstcon.h>
#include <s5p4418-gpio.h>
#include <s5p4418/reg-pwm.h>

#define TCON_CHANNEL(ch)		(ch ? ch * 4 + 4 : 0)
#define TCON_START(ch)			(0x1 << TCON_CHANNEL(ch))
#define TCON_MANUALUPDATE(ch)	(0x2 << TCON_CHANNEL(ch))
#define TCON_INVERT(ch)			(0x4 << TCON_CHANNEL(ch))
#define TCON_AUTORELOAD(ch)		(0x8 << TCON_CHANNEL(ch))

struct s5p4418_pwm_data_t
{
	const char * name;
	int ch;
	int pin;
	int pincfg;
	physical_addr_t phys;
};

struct s5p4418_pwm_pdata_t
{
	const char * name;
	int ch;
	int pin;
	int pincfg;
	virtual_addr_t virt;
};

static struct s5p4418_pwm_data_t datas[] = {
	{
		.name	= "pwm0",
		.ch		= 0,
		.pin	= S5P4418_GPIOD(1),
		.pincfg	= 0x1,
		.phys	= S5P4418_PWM_BASE,
	}, {
		.name	= "pwm1",
		.ch		= 1,
		.pin	= S5P4418_GPIOC(13),
		.pincfg	= 0x2,
		.phys	= S5P4418_PWM_BASE,
	},
	{
		.name	= "pwm2",
		.ch		= 2,
		.pin	= S5P4418_GPIOC(14),
		.pincfg	= 0x2,
		.phys	= S5P4418_PWM_BASE,
	},
	{
		.name	= "pwm3",
		.ch		= 3,
		.pin	= S5P4418_GPIOD(0),
		.pincfg	= 0x2,
		.phys	= S5P4418_PWM_BASE,
	},
};

static u64_t s5p4418_pwm_calc_tin(struct pwm_t * pwm, u32_t period)
{
	struct s5p4418_pwm_pdata_t * pdat = (struct s5p4418_pwm_pdata_t *)pwm->priv;
	u64_t rate, freq = 1000000000L / period;
	u8_t div, shift;

	if(pdat->ch < 2)
		rate = clk_get_rate("DIV-PWM-PRESCALER0");
	else
		rate = clk_get_rate("DIV-PWM-PRESCALER1");

	for(div = 0; div < 4; div++)
	{
		if((rate >> div) <= freq)
			break;
	}

	shift = pdat->ch * 4;
	write32(pdat->virt + PWM_TCFG1, (read32(pdat->virt + PWM_TCFG1) & ~(0xf<<shift)) | (div<<shift));

	return (rate >> div);
}

static void s5p4418_pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period, bool_t polarity)
{
	struct s5p4418_pwm_pdata_t * pdat = (struct s5p4418_pwm_pdata_t *)pwm->priv;
	u64_t rate;
	u32_t tcnt, tcmp;
	u32_t tcon;

	if((pwm->__duty != duty) || (pwm->__period != period))
	{
		rate = s5p4418_pwm_calc_tin(pwm, period);

		if(pwm->__duty != duty)
		{
			tcmp = rate * duty / 1000000000L;
			write32(pdat->virt + PWM_TCMPB(pdat->ch), tcmp);
		}

		if(pwm->__period != period)
		{
			tcnt = rate * period / 1000000000L;
			write32(pdat->virt + PWM_TCNTB(pdat->ch), tcnt);
		}

		tcon = read32(pdat->virt + PWM_TCON);
		tcon |= TCON_MANUALUPDATE(pdat->ch);
		write32(pdat->virt + PWM_TCON, tcon);

		tcon &= ~TCON_MANUALUPDATE(pdat->ch);
		write32(pdat->virt + PWM_TCON, tcon);
	}

	if(pwm->__polarity != polarity)
	{
		tcon = read32(pdat->virt + PWM_TCON);
		if(polarity)
			tcon |= TCON_INVERT(pdat->ch);
		else
			tcon &= ~TCON_INVERT(pdat->ch);
		write32(pdat->virt + PWM_TCON, tcon);
	}
}

static void s5p4418_pwm_enable(struct pwm_t * pwm)
{
	struct s5p4418_pwm_pdata_t * pdat = (struct s5p4418_pwm_pdata_t *)pwm->priv;
	u32_t tcon;

	gpio_set_cfg(pdat->pin, pdat->pincfg);
	if(pdat->ch < 2)
		clk_enable("DIV-PWM-PRESCALER0");
	else
		clk_enable("DIV-PWM-PRESCALER1");

	tcon = read32(pdat->virt + PWM_TCON);
	tcon &= ~(TCON_AUTORELOAD(pdat->ch) | TCON_START(pdat->ch));
	tcon |= TCON_MANUALUPDATE(pdat->ch);
	write32(pdat->virt + PWM_TCON, tcon);

	tcon = read32(pdat->virt + PWM_TCON);
	tcon &= ~TCON_MANUALUPDATE(pdat->ch);
	tcon |= TCON_AUTORELOAD(pdat->ch) | TCON_START(pdat->ch);
	write32(pdat->virt + PWM_TCON, tcon);
}

static void s5p4418_pwm_disable(struct pwm_t * pwm)
{
	struct s5p4418_pwm_pdata_t * pdat = (struct s5p4418_pwm_pdata_t *)pwm->priv;
	u32_t tcon;

	tcon = read32(pdat->virt + PWM_TCON);
	tcon &= ~(TCON_AUTORELOAD(pdat->ch) | TCON_START(pdat->ch));
	write32(pdat->virt + PWM_TCON, tcon);

	if(pdat->ch < 2)
		clk_disable("DIV-PWM-PRESCALER0");
	else
		clk_disable("DIV-PWM-PRESCALER1");
}

static __init void s5p4418_pwm_init(void)
{
	struct s5p4418_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	int i;

	s5p4418_ip_reset(RESET_ID_PWM, 0);

	for(i = 0; i < ARRAY_SIZE(datas); i++)
	{
		pdat = malloc(sizeof(struct s5p4418_pwm_pdata_t));
		if(!pdat)
			continue;

		pwm = malloc(sizeof(struct pwm_t));
		if(!pwm)
		{
			free(pdat);
			continue;
		}

		pdat->name = datas[i].name;
		pdat->ch = datas[i].ch;
		pdat->pin = datas[i].pin;
		pdat->pincfg = datas[i].pincfg;
		pdat->virt = phys_to_virt(datas[i].phys);

		pwm->name = pdat->name;
		pwm->config = s5p4418_pwm_config;
		pwm->enable = s5p4418_pwm_enable;
		pwm->disable = s5p4418_pwm_disable;
		pwm->priv = pdat;

		register_pwm(pwm);
	}
}
core_initcall(s5p4418_pwm_init);
