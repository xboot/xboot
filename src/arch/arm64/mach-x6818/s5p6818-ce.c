/*
 * s5p6818-ce.c
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
#include <s5p6818-irq.h>
#include <s5p6818-timer.h>

#define CE_TIMER_CHANNEL	(0)

struct clockevent_pdata_t
{
	int irq;
	physical_addr_t phys;
	virtual_addr_t virt;
};

static void ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;
	s5p6818_timer_irq_clear(pdat->virt, CE_TIMER_CHANNEL);
	ce->handler(ce, ce->data);
}

static bool_t ce_init(struct clockevent_t * ce)
{
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;
	u64_t rate;

	s5p6818_timer_reset();

	if(!request_irq(pdat->irq, ce_interrupt, IRQ_TYPE_NONE, ce))
		return FALSE;

	/* 9.375MHZ - 107ns */
	s5p6818_timer_enable(pdat->virt, CE_TIMER_CHANNEL, 1);
	rate = s5p6818_timer_calc_tin(pdat->virt, CE_TIMER_CHANNEL, 107);
	clockevent_calc_mult_shift(ce, rate, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	s5p6818_timer_count(pdat->virt, CE_TIMER_CHANNEL, 0);
	s5p6818_timer_stop(pdat->virt, CE_TIMER_CHANNEL);

	return TRUE;
}

static bool_t ce_next(struct clockevent_t * ce, u64_t evt)
{
	struct clockevent_pdata_t * pdat = (struct clockevent_pdata_t *)ce->priv;

	s5p6818_timer_count(pdat->virt, CE_TIMER_CHANNEL, (evt & 0xffffffff));
	s5p6818_timer_start(pdat->virt, CE_TIMER_CHANNEL, 1);
	return TRUE;
}

static struct clockevent_pdata_t pdata = {
	.irq 	= S5P6818_IRQ_TIMER0,
	.phys	= S5P6818_TIMER_BASE,
};

static struct clockevent_t ce = {
	.name	= "s5p6818-ce",
	.init	= ce_init,
	.next	= ce_next,
	.priv	= &pdata,
};

static __init void s5p6818_clockevent_init(void)
{
	pdata.virt = phys_to_virt(pdata.phys);
	register_clockevent(&ce);
}
core_initcall(s5p6818_clockevent_init);
