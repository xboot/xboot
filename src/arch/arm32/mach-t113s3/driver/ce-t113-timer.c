/*
 * driver/ce-t113-timer.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

#define TIMER_IRQ_EN	(0x00)
#define TIMER_IRQ_STA	(0x04)
#define TIMER_CTRL(x)	((x + 1) * 0x10 + 0x00)
#define TIMER_INTV(x)	((x + 1) * 0x10 + 0x04)
#define TIMER_CUR(x)	((x + 1) * 0x10 + 0x08)

struct ce_t113_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
};

static void ce_t113_timer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_t113_timer_pdata_t * pdat = (struct ce_t113_timer_pdata_t *)ce->priv;
	write32(pdat->virt + TIMER_IRQ_STA, 1 << 0);
	ce->handler(ce, ce->data);
}

static bool_t ce_t113_timer_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_t113_timer_pdata_t * pdat = (struct ce_t113_timer_pdata_t *)ce->priv;
	u32_t val;

	write32(pdat->virt + TIMER_INTV(0), (evt & 0xffffffff));
	val = read32(pdat->virt + TIMER_CTRL(0));
	val &= ~(0x3 << 0);
	val |= 0x2 << 0;
	write32(pdat->virt + TIMER_CTRL(0), val);
	val &= ~(0x3 << 0);
	val |= 0x1 << 0;
	write32(pdat->virt + TIMER_CTRL(0), val);
	return TRUE;
}

static struct device_t * ce_t113_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_t113_timer_pdata_t * pdat;
	struct clockevent_t * ce;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	u32_t val;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct ce_t113_timer_pdata_t));
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
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk), 10);
	ce->name = alloc_device_name(dt_read_name(n), -1);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	ce->next = ce_t113_timer_next;
	ce->priv = pdat;

	if(!request_irq(pdat->irq, ce_t113_timer_interrupt, IRQ_TYPE_NONE, ce))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);

		free(ce->priv);
		free(ce);
		return NULL;
	}
	write32(pdat->virt + TIMER_IRQ_EN, read32(pdat->virt + TIMER_IRQ_EN) | (1 << 0));
	write32(pdat->virt + TIMER_IRQ_STA, 1 << 0);
	val = read32(pdat->virt + TIMER_CTRL(0));
	val &= ~((0x1 << 7) | (0x3 << 0));
	val |= (0x1 << 7) | (0x0 << 0);
	write32(pdat->virt + TIMER_CTRL(0), val);

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

static void ce_t113_timer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_t113_timer_pdata_t * pdat = (struct ce_t113_timer_pdata_t *)ce->priv;

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

static void ce_t113_timer_suspend(struct device_t * dev)
{
}

static void ce_t113_timer_resume(struct device_t * dev)
{
}

static struct driver_t ce_t113_timer = {
	.name		= "ce-t113-timer",
	.probe		= ce_t113_timer_probe,
	.remove		= ce_t113_timer_remove,
	.suspend	= ce_t113_timer_suspend,
	.resume		= ce_t113_timer_resume,
};

static __init void ce_t113_timer_driver_init(void)
{
	register_driver(&ce_t113_timer);
}

static __exit void ce_t113_timer_driver_exit(void)
{
	unregister_driver(&ce_t113_timer);
}

driver_initcall(ce_t113_timer_driver_init);
driver_exitcall(ce_t113_timer_driver_exit);
