/*
 * driver/pwm-f1c100s.c
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <pwm/pwm.h>

#define PWM_CTRL		(0x00)
#define PWM_BIT(x, bit)	((1 << bit) << ((x) * 15))
#define PWM_PERIOD(x)	(((x) * 0x4) + 0x4)

static const u32_t prescaler_table[] = {
	120,
	180,
	240,
	360,
	480,
	0,
	0,
	0,
	12000,
	24000,
	36000,
	48000,
	72000,
	0,
	0,
	0,
};

struct pwm_f1c100s_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int channel;
	int pwm;
	int pwmcfg;
};

static void pwm_f1c100s_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_f1c100s_pdata_t * pdat = (struct pwm_f1c100s_pdata_t *)pwm->priv;
	u64_t rate = clk_get_rate(pdat->clk);
	u64_t div;
	u32_t prd, dty, gate;
	u32_t prescaler = 0;
	u32_t ctrl;

	if(pwm->__duty != duty || pwm->__period != period)
	{
		prescaler = 0xf;
		div = rate * period + 1000000000 / 2;
		div = div / 1000000000;
		if(div - 1 > 0xffff)
			prescaler = 0;

		if(prescaler == 0)
		{
			for(prescaler = 0; prescaler < 0xf; prescaler++)
			{
				if(!prescaler_table[prescaler])
					continue;
				div = rate;
				div = div / prescaler_table[prescaler];
				div = div * period;
				div = div / 1000000000;
				if(div - 1 <= 0xffff)
					break;
			}

			if(div - 1 > 0xffff)
			{
				return;
			}
		}

		prd = div;
		div *= duty;
		div = div / period;
		dty = div;

		gate = (read32(pdat->virt + PWM_CTRL) & PWM_BIT(pdat->channel, 6)) ? 1 : 0;
		if(gate)
		{
			ctrl = read32(pdat->virt + PWM_CTRL);
			ctrl &= ~PWM_BIT(pdat->channel, 6);
			write32(pdat->virt + PWM_CTRL, ctrl);
		}
		ctrl = read32(pdat->virt + PWM_CTRL);
		ctrl &= ~(0xf << (pdat->channel * 15));
		ctrl |= (prescaler << (pdat->channel * 15));
		write32(pdat->virt + PWM_CTRL, ctrl);
		write32(pdat->virt + PWM_PERIOD(pdat->channel), ((prd - 1) << 16) | (dty & 0xffff));
		if(gate)
		{
			ctrl = read32(pdat->virt + PWM_CTRL);
			ctrl |= PWM_BIT(pdat->channel, 6);
			write32(pdat->virt + PWM_CTRL, ctrl);
		}
	}

	if(pwm->__polarity != polarity)
	{
		ctrl = read32(pdat->virt + PWM_CTRL);
		if(polarity)
			ctrl |= PWM_BIT(pdat->channel, 5);
		else
			ctrl &= ~PWM_BIT(pdat->channel, 5);
		write32(pdat->virt + PWM_CTRL, ctrl);
	}
}

static void pwm_f1c100s_enable(struct pwm_t * pwm)
{
	struct pwm_f1c100s_pdata_t * pdat = (struct pwm_f1c100s_pdata_t *)pwm->priv;
	u32_t ctrl;

	if((pdat->pwm >= 0) && (pdat->pwmcfg >= 0))
		gpio_set_cfg(pdat->pwm, pdat->pwmcfg);
	clk_enable(pdat->clk);

	ctrl = read32(pdat->virt + PWM_CTRL);
	ctrl |= PWM_BIT(pdat->channel, 4);
	ctrl |= PWM_BIT(pdat->channel, 6);
	write32(pdat->virt + PWM_CTRL, ctrl);
}

static void pwm_f1c100s_disable(struct pwm_t * pwm)
{
	struct pwm_f1c100s_pdata_t * pdat = (struct pwm_f1c100s_pdata_t *)pwm->priv;
	u32_t ctrl;

	ctrl = read32(pdat->virt + PWM_CTRL);
	ctrl &= ~PWM_BIT(pdat->channel, 4);
	ctrl &= ~PWM_BIT(pdat->channel, 6);
	write32(pdat->virt + PWM_CTRL, ctrl);
	clk_disable(pdat->clk);
}

static struct device_t * pwm_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_f1c100s_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 1)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct pwm_f1c100s_pdata_t));
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
	pwm->config = pwm_f1c100s_config;
	pwm->enable = pwm_f1c100s_enable;
	pwm->disable = pwm_f1c100s_disable;
	pwm->priv = pdat;

	write32(pdat->virt + PWM_CTRL, read32(pdat->virt + PWM_CTRL) &~(0x3fff << (pdat->channel * 15)));
	write32(pdat->virt + PWM_PERIOD(pdat->channel), 0);

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

static void pwm_f1c100s_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_f1c100s_pdata_t * pdat = (struct pwm_f1c100s_pdata_t *)pwm->priv;

	if(pwm && unregister_pwm(pwm))
	{
		free(pdat->clk);

		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_f1c100s_suspend(struct device_t * dev)
{
}

static void pwm_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t pwm_f1c100s = {
	.name		= "pwm-f1c100s",
	.probe		= pwm_f1c100s_probe,
	.remove		= pwm_f1c100s_remove,
	.suspend	= pwm_f1c100s_suspend,
	.resume		= pwm_f1c100s_resume,
};

static __init void pwm_f1c100s_driver_init(void)
{
	register_driver(&pwm_f1c100s);
}

static __exit void pwm_f1c100s_driver_exit(void)
{
	unregister_driver(&pwm_f1c100s);
}

driver_initcall(pwm_f1c100s_driver_init);
driver_exitcall(pwm_f1c100s_driver_exit);
