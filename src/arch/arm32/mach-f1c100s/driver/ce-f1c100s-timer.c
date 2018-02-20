/*
 * driver/ce-f1c100s-timer.c
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
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>

#define TIMER_IRQ_EN	(0x00)
#define TIMER_IRQ_STA	(0x04)
#define TIMER_CTRL(x)	((x + 1) * 0x10 + 0x00)
#define TIMER_INTV(x)	((x + 1) * 0x10 + 0x04)
#define TIMER_CUR(x)	((x + 1) * 0x10 + 0x08)

struct ce_f1c100s_timer_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
};

static void ce_f1c100s_timer_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct ce_f1c100s_timer_pdata_t * pdat = (struct ce_f1c100s_timer_pdata_t *)ce->priv;
	write32(pdat->virt + TIMER_IRQ_STA, 1 << 0);
	ce->handler(ce, ce->data);
}

static bool_t ce_f1c100s_timer_next(struct clockevent_t * ce, u64_t evt)
{
	struct ce_f1c100s_timer_pdata_t * pdat = (struct ce_f1c100s_timer_pdata_t *)ce->priv;
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

static struct device_t * ce_f1c100s_timer_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ce_f1c100s_timer_pdata_t * pdat;
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

	pdat = malloc(sizeof(struct ce_f1c100s_timer_pdata_t));
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
	ce->next = ce_f1c100s_timer_next;
	ce->priv = pdat;

	if(!request_irq(pdat->irq, ce_f1c100s_timer_interrupt, IRQ_TYPE_NONE, ce))
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

	if(!register_clockevent(&dev, ce))
	{
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

static void ce_f1c100s_timer_remove(struct device_t * dev)
{
	struct clockevent_t * ce = (struct clockevent_t *)dev->priv;
	struct ce_f1c100s_timer_pdata_t * pdat = (struct ce_f1c100s_timer_pdata_t *)ce->priv;

	if(ce && unregister_clockevent(ce))
	{
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(ce->name);
		free(ce->priv);
		free(ce);
	}
}

static void ce_f1c100s_timer_suspend(struct device_t * dev)
{
}

static void ce_f1c100s_timer_resume(struct device_t * dev)
{
}

static struct driver_t ce_f1c100s_timer = {
	.name		= "ce-f1c100s-timer",
	.probe		= ce_f1c100s_timer_probe,
	.remove		= ce_f1c100s_timer_remove,
	.suspend	= ce_f1c100s_timer_suspend,
	.resume		= ce_f1c100s_timer_resume,
};

static __init void ce_f1c100s_timer_driver_init(void)
{
	register_driver(&ce_f1c100s_timer);
}

static __exit void ce_f1c100s_timer_driver_exit(void)
{
	unregister_driver(&ce_f1c100s_timer);
}

driver_initcall(ce_f1c100s_timer_driver_init);
driver_exitcall(ce_f1c100s_timer_driver_exit);
