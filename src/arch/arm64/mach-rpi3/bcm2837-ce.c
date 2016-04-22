/*
 * bcm2837-ce.c
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

#include <xboot.h>
#include <clockevent/clockevent.h>
#include <bcm2837-irq.h>
#include <bcm2837/reg-arm-timer.h>

struct clockevent_pdata_t
{
	const char * clk;
	int irq;
	physical_addr_t phys;
	virtual_addr_t virt;
};

static void ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;
	write32(pdat->virt + ARM_TIMER_CLI, 0);
	write32(pdat->virt + ARM_TIMER_CTL, 0);
	ce->handler(ce, ce->data);
}

static bool_t ce_init(struct clockevent_t * ce)
{
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;

	clk_enable(pdat->clk);
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk) / 250, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);

	if(!request_irq(pdat->irq, ce_interrupt, IRQ_TYPE_NONE, ce))
	{
		clk_disable(pdat->clk);
		return FALSE;
	}
	write32(pdat->virt + ARM_TIMER_DIV, (250 - 1));
	write32(pdat->virt + ARM_TIMER_RLD, 0);
	write32(pdat->virt + ARM_TIMER_LOD, 0);
	write32(pdat->virt + ARM_TIMER_CLI, 0);
	write32(pdat->virt + ARM_TIMER_CTL, 0);
	return TRUE;
}

static bool_t ce_next(struct clockevent_t * ce, u64_t evt)
{
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;

	write32(pdat->virt + ARM_TIMER_RLD, (evt & 0xffffffff));
	write32(pdat->virt + ARM_TIMER_LOD, (evt & 0xffffffff));
	write32(pdat->virt + ARM_TIMER_CTL, (1 << 1) | (1 << 5) | (1 << 7));
	return TRUE;
}

static struct clockevent_pdata_t pdata = {
	.clk	= "arm-timer-clk",
	.irq 	= BCM2837_IRQ_ARM_TIMER,
	.phys	= BCM2837_ARM_TIMER_BASE,
};

static struct clockevent_t ce = {
	.name	= "bcm2837-ce",
	.init	= ce_init,
	.next	= ce_next,
	.priv	= &pdata,
};

static __init void bcm2837_clockevent_init(void)
{
	pdata.virt = phys_to_virt(pdata.phys);
	register_clockevent(&ce);
}
core_initcall(bcm2837_clockevent_init);
