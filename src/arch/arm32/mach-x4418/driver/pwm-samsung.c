/*
 * driver/pwm-samsung.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#define PWM_TCFG0				(0x00)
#define PWM_TCFG1				(0x04)
#define PWM_TCON				(0x08)
#define PWM_TSTAT				(0x44)
#define PWM_TCNTB(x)			((x + 1) * 0xC + 0x00)
#define PWM_TCMPB(x)			((x + 1) * 0xC + 0x04)
#define PWM_TCNTO(x)			((x + 1) * 0xC + 0x08)
#define TCON_START(x)			(0x1 << (x ? x * 4 + 4 : 0))
#define TCON_MANUALUPDATE(x)	(0x2 << (x ? x * 4 + 4 : 0))
#define TCON_INVERT(x)			(0x4 << (x ? x * 4 + 4 : 0))
#define TCON_AUTORELOAD(x)		(0x8 << (x ? x * 4 + 4 : 0))

struct pwm_samsung_pdata_t
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

static u64_t pwm_samsung_calc_tin(struct pwm_t * pwm, u32_t period)
{
	struct pwm_samsung_pdata_t * pdat = (struct pwm_samsung_pdata_t *)pwm->priv;
	u64_t rate, freq = 1000000000L / period;
	u8_t div, shift;

	rate = clk_get_rate(pdat->clk);
	for(div = 0; div < 4; div++)
	{
		if((rate >> div) <= freq)
			break;
	}

	shift = pdat->channel * 4;
	write32(pdat->virt + PWM_TCFG1, (read32(pdat->virt + PWM_TCFG1) & ~(0xf<<shift)) | (div<<shift));

	return (rate >> div);
}

static void pwm_samsung_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_samsung_pdata_t * pdat = (struct pwm_samsung_pdata_t *)pwm->priv;
	u64_t rate;
	u32_t tcnt, tcmp;
	u32_t tcon;

	if(pdat->duty != duty || pdat->period != period)
	{
		rate = pwm_samsung_calc_tin(pwm, period);
		if(pdat->duty != duty)
		{
			pdat->duty = duty;
			tcmp = rate * duty / 1000000000L;
			write32(pdat->virt + PWM_TCMPB(pdat->channel), tcmp);
		}
		if(pdat->period != period)
		{
			pdat->period = period;
			tcnt = rate * period / 1000000000L;
			write32(pdat->virt + PWM_TCNTB(pdat->channel), tcnt);
		}
		tcon = read32(pdat->virt + PWM_TCON);
		tcon |= TCON_MANUALUPDATE(pdat->channel);
		write32(pdat->virt + PWM_TCON, tcon);
		tcon &= ~TCON_MANUALUPDATE(pdat->channel);
		write32(pdat->virt + PWM_TCON, tcon);
	}
	if(pdat->polarity != polarity)
	{
		pdat->polarity = polarity;
		tcon = read32(pdat->virt + PWM_TCON);
		if(polarity)
			tcon |= TCON_INVERT(pdat->channel);
		else
			tcon &= ~TCON_INVERT(pdat->channel);
		write32(pdat->virt + PWM_TCON, tcon);
	}
}

static void pwm_samsung_enable(struct pwm_t * pwm)
{
	struct pwm_samsung_pdata_t * pdat = (struct pwm_samsung_pdata_t *)pwm->priv;
	u32_t tcon;

	if(pdat->enable != 1)
	{
		pdat->enable = 1;
		if((pdat->pwm >= 0) && (pdat->pwmcfg >= 0))
			gpio_set_cfg(pdat->pwm, pdat->pwmcfg);
		clk_enable(pdat->clk);
		tcon = read32(pdat->virt + PWM_TCON);
		tcon &= ~(TCON_AUTORELOAD(pdat->channel) | TCON_START(pdat->channel));
		tcon |= TCON_MANUALUPDATE(pdat->channel);
		write32(pdat->virt + PWM_TCON, tcon);
		tcon = read32(pdat->virt + PWM_TCON);
		tcon &= ~TCON_MANUALUPDATE(pdat->channel);
		tcon |= TCON_AUTORELOAD(pdat->channel) | TCON_START(pdat->channel);
		write32(pdat->virt + PWM_TCON, tcon);
	}
}

static void pwm_samsung_disable(struct pwm_t * pwm)
{
	struct pwm_samsung_pdata_t * pdat = (struct pwm_samsung_pdata_t *)pwm->priv;
	u32_t tcon;

	if(pdat->enable != 0)
	{
		pdat->enable = 0;
		tcon = read32(pdat->virt + PWM_TCON);
		tcon &= ~(TCON_AUTORELOAD(pdat->channel) | TCON_START(pdat->channel));
		write32(pdat->virt + PWM_TCON, tcon);
		clk_disable(pdat->clk);
	}
}

static struct device_t * pwm_samsung_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_samsung_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 3)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct pwm_samsung_pdata_t));
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
	pwm->config = pwm_samsung_config;
	pwm->enable = pwm_samsung_enable;
	pwm->disable = pwm_samsung_disable;
	pwm->priv = pdat;

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

static void pwm_samsung_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_samsung_pdata_t * pdat = (struct pwm_samsung_pdata_t *)pwm->priv;

	if(pwm)
	{
		unregister_pwm(pwm);
		free(pdat->clk);
		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_samsung_suspend(struct device_t * dev)
{
}

static void pwm_samsung_resume(struct device_t * dev)
{
}

static struct driver_t pwm_samsung = {
	.name		= "pwm-samsung",
	.probe		= pwm_samsung_probe,
	.remove		= pwm_samsung_remove,
	.suspend	= pwm_samsung_suspend,
	.resume		= pwm_samsung_resume,
};

static __init void pwm_samsung_driver_init(void)
{
	register_driver(&pwm_samsung);
}

static __exit void pwm_samsung_driver_exit(void)
{
	unregister_driver(&pwm_samsung);
}

driver_initcall(pwm_samsung_driver_init);
driver_exitcall(pwm_samsung_driver_exit);
