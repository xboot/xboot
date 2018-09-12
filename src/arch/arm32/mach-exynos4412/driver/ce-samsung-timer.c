/*
 * driver/ce-samsung-timer.c
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
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>
#include <samsung-timer.h>

struct ce_samsung_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int channel;
};

static void ce_samsung_timer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_samsung_timer_pdata_t * pdat = (struct ce_samsung_timer_pdata_t *)ce->priv;
	samsung_timer_irq_clear(pdat->virt, pdat->channel);
	ce->handler(ce, ce->data);
}

static bool_t ce_samsung_timer_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_samsung_timer_pdata_t * pdat = (struct ce_samsung_timer_pdata_t *)ce->priv;

	samsung_timer_count(pdat->virt, pdat->channel, (evt & 0xffffffff));
	samsung_timer_start(pdat->virt, pdat->channel, 1);
	return TRUE;
}

static struct device_t * ce_samsung_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_samsung_timer_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int channel = dt_read_int(n, "timer-channel", -1);
	u64_t rate;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if(channel < 0 || channel > 3)
		return NULL;

	pdat = malloc(sizeof(struct ce_samsung_timer_pdata_t));
	if(!pdat)
		return NULL;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->channel = channel;

	clk_enable(pdat->clk);
	rate = samsung_timer_calc_tin(pdat->virt, pdat->clk, pdat->channel, 107);
	clockevent_calc_mult_shift(ce, rate, 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	ce->next = ce_samsung_timer_next;
	ce->priv = pdat;

	if(!request_irq(pdat->irq, ce_samsung_timer_interrupt, IRQ_TYPE_NONE, ce))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free(ce->priv);
		free(ce);
		return NULL;
	}
	samsung_timer_enable(pdat->virt, pdat->channel, 1);
	samsung_timer_count(pdat->virt, pdat->channel, 0);
	samsung_timer_stop(pdat->virt, pdat->channel);

	if(!register_clockevent(&dev, ce))
	{
		samsung_timer_irq_clear(pdat->virt, pdat->channel);
		samsung_timer_stop(pdat->virt, pdat->channel);
		samsung_timer_disable(pdat->virt, pdat->channel);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ce_samsung_timer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_samsung_timer_pdata_t * pdat = (struct ce_samsung_timer_pdata_t *)ce->priv;

	if(ce && unregister_clockevent(ce))
	{
		samsung_timer_irq_clear(pdat->virt, pdat->channel);
		samsung_timer_stop(pdat->virt, pdat->channel);
		samsung_timer_disable(pdat->virt, pdat->channel);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_samsung_timer_suspend(struct device_t * dev)
{
}

static void ce_samsung_timer_resume(struct device_t * dev)
{
}

static struct driver_t ce_samsung_timer = {
	.name		= "ce-samsung-timer",
	.probe		= ce_samsung_timer_probe,
	.remove		= ce_samsung_timer_remove,
	.suspend	= ce_samsung_timer_suspend,
	.resume		= ce_samsung_timer_resume,
};

static __init void ce_samsung_timer_driver_init(void)
{
	register_driver(&ce_samsung_timer);
}

static __exit void ce_samsung_timer_driver_exit(void)
{
	unregister_driver(&ce_samsung_timer);
}

driver_initcall(ce_samsung_timer_driver_init);
driver_exitcall(ce_samsung_timer_driver_exit);
