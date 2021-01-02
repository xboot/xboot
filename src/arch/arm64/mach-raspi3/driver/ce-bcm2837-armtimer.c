/*
 * driver/ce-bcm2837-armtimer.c
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
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>

enum {
	ARM_TIMER_LOD = 0x00,
	ARM_TIMER_VAL = 0x04,
	ARM_TIMER_CTL = 0x08,
	ARM_TIMER_CLI = 0x0c,
	ARM_TIMER_RIS = 0x10,
	ARM_TIMER_MIS = 0x14,
	ARM_TIMER_RLD = 0x18,
	ARM_TIMER_DIV = 0x1c,
	ARM_TIMER_CNT = 0x20,
};

struct ce_bcm2837_armtimer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
};

static void ce_bcm2837_armtimer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_bcm2837_armtimer_pdata_t * pdat = (struct ce_bcm2837_armtimer_pdata_t *)ce->priv;
	write32(pdat->virt + ARM_TIMER_CLI, 0);
	write32(pdat->virt + ARM_TIMER_CTL, 0);
	ce->handler(ce, ce->data);
}

static bool_t ce_bcm2837_armtimer_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_bcm2837_armtimer_pdata_t * pdat = (struct ce_bcm2837_armtimer_pdata_t *)ce->priv;

	write32(pdat->virt + ARM_TIMER_RLD, (evt & 0xffffffff));
	write32(pdat->virt + ARM_TIMER_LOD, (evt & 0xffffffff));
	write32(pdat->virt + ARM_TIMER_CTL, (1 << 1) | (1 << 5) | (1 << 7));
	return TRUE;
}

static struct device_t * ce_bcm2837_armtimer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_bcm2837_armtimer_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct ce_bcm2837_armtimer_pdata_t));
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

	clk_enable(pdat->clk);
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk) / 250, 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	ce->next = ce_bcm2837_armtimer_next;
	ce->priv = pdat;

	if(!request_irq(pdat->irq, ce_bcm2837_armtimer_interrupt, IRQ_TYPE_NONE, ce))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free(ce->priv);
		free(ce);
		return NULL;
	}
	write32(pdat->virt + ARM_TIMER_DIV, (250 - 1));
	write32(pdat->virt + ARM_TIMER_RLD, 0);
	write32(pdat->virt + ARM_TIMER_LOD, 0);
	write32(pdat->virt + ARM_TIMER_CLI, 0);
	write32(pdat->virt + ARM_TIMER_CTL, 0);

	if(!(dev = register_clockevent(ce, drv)))
	{
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
		return NULL;
	}
	return dev;
}

static void ce_bcm2837_armtimer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_bcm2837_armtimer_pdata_t * pdat = (struct ce_bcm2837_armtimer_pdata_t *)ce->priv;

	if(ce)
	{
		unregister_clockevent(ce);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_bcm2837_armtimer_suspend(struct device_t * dev)
{
}

static void ce_bcm2837_armtimer_resume(struct device_t * dev)
{
}

static struct driver_t ce_bcm2837_armtimer = {
	.name		= "ce-bcm2837-armtimer",
	.probe		= ce_bcm2837_armtimer_probe,
	.remove		= ce_bcm2837_armtimer_remove,
	.suspend	= ce_bcm2837_armtimer_suspend,
	.resume		= ce_bcm2837_armtimer_resume,
};

static __init void ce_bcm2837_armtimer_driver_init(void)
{
	register_driver(&ce_bcm2837_armtimer);
}

static __exit void ce_bcm2837_armtimer_driver_exit(void)
{
	unregister_driver(&ce_bcm2837_armtimer);
}

driver_initcall(ce_bcm2837_armtimer_driver_init);
driver_exitcall(ce_bcm2837_armtimer_driver_exit);
