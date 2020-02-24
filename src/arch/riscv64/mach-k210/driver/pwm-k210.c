/*
 * driver/pwm-k210.c
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
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <pwm/pwm.h>

#define PWM_LOAD(x)			(((x) * 0x14) + 0x0)
#define PWM_VALUE(x)		(((x) * 0x14) + 0x4)
#define PWM_CTRL(x)			(((x) * 0x14) + 0x8)
#define PWM_LOAD2(x)		(((x) * 0x4) + 0xb0)
#define PWM_INT_STATUS		(0xa0)
#define PWM_INT_CLR			(0xa4)
#define PWM_INT_RAW_STATUS	(0xa8)
#define PWM_VERSION			(0xac)

struct pwm_k210_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int reset;
	int channel;
	int pwm;
	int pwmcfg;

	int enable;
	int duty;
	int period;
	int polarity;
};

static void pwm_k210_config(struct pwm_t * pwm, int duty, int period, int polarity)
{
	struct pwm_k210_pdata_t * pdat = (struct pwm_k210_pdata_t *)pwm->priv;
	u64_t rate;

	if((pdat->duty != duty) || (pdat->period != period) || (pdat->polarity != polarity))
	{
		pdat->duty = duty;
		pdat->period = period;
		pdat->polarity = polarity;
		rate = clk_get_rate(pdat->clk);
		write32(pdat->virt + PWM_LOAD(pdat->channel), (u32_t)(rate * duty / 1000000000L));
		write32(pdat->virt + PWM_LOAD2(pdat->channel), (u32_t)(rate * period / 1000000000L));
	}
}

static void pwm_k210_enable(struct pwm_t * pwm)
{
	struct pwm_k210_pdata_t * pdat = (struct pwm_k210_pdata_t *)pwm->priv;
	if(pdat->enable != 1)
	{
		pdat->enable = 1;
		write32(pdat->virt + PWM_CTRL(pdat->channel), (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0));
	}
}

static void pwm_k210_disable(struct pwm_t * pwm)
{
	struct pwm_k210_pdata_t * pdat = (struct pwm_k210_pdata_t *)pwm->priv;
	if(pdat->enable != 0)
	{
		pdat->enable = 0;
		write32(pdat->virt + PWM_CTRL(pdat->channel), (1 << 2));
	}
}

static struct device_t * pwm_k210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct pwm_k210_pdata_t * pdat;
	struct pwm_t * pwm;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 3)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct pwm_k210_pdata_t));
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
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->channel = channel;
	pdat->pwm = dt_read_int(n, "pwm-gpio", -1);
	pdat->pwmcfg = dt_read_int(n, "pwm-gpio-config", -1);
	pdat->enable = -1;
	pdat->duty = 500 * 1000;
	pdat->period = 1000 * 1000;
	pdat->polarity = 0;

	pwm->name = alloc_device_name(dt_read_name(n), -1);
	pwm->config = pwm_k210_config;
	pwm->enable = pwm_k210_enable;
	pwm->disable = pwm_k210_disable;
	pwm->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	if((pdat->pwm >= 0) && (pdat->pwmcfg >= 0))
		gpio_set_cfg(pdat->pwm, pdat->pwmcfg);

	write32(pdat->virt + PWM_LOAD(pdat->channel), 500);
	write32(pdat->virt + PWM_LOAD2(pdat->channel), 1000);
	write32(pdat->virt + PWM_CTRL(pdat->channel), (1 << 2));

	if(!(dev = register_pwm(pwm, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
		return NULL;
	}
	return dev;
}

static void pwm_k210_remove(struct device_t * dev)
{
	struct pwm_t * pwm = (struct pwm_t *)dev->priv;
	struct pwm_k210_pdata_t * pdat = (struct pwm_k210_pdata_t *)pwm->priv;

	if(pwm)
	{
		unregister_pwm(pwm);
		free(pdat->clk);
		free_device_name(pwm->name);
		free(pwm->priv);
		free(pwm);
	}
}

static void pwm_k210_suspend(struct device_t * dev)
{
}

static void pwm_k210_resume(struct device_t * dev)
{
}

static struct driver_t pwm_k210 = {
	.name		= "pwm-k210",
	.probe		= pwm_k210_probe,
	.remove		= pwm_k210_remove,
	.suspend	= pwm_k210_suspend,
	.resume		= pwm_k210_resume,
};

static __init void pwm_k210_driver_init(void)
{
	register_driver(&pwm_k210);
}

static __exit void pwm_k210_driver_exit(void)
{
	unregister_driver(&pwm_k210);
}

driver_initcall(pwm_k210_driver_init);
driver_exitcall(pwm_k210_driver_exit);
