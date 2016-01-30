/*
 * realview-ce.c
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
#include <realview/reg-timer.h>

struct realview_clockevent_pdata_t
{
	char * clk;
	char * irq;
	virtual_addr_t regbase;
};

static void realview_ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	struct realview_clockevent_pdata_t * pdat = (struct realview_clockevent_pdata_t *)ce->priv;
	write32(pdat->regbase + TIMER_ICLR, 0x0);
	ce->handler(ce, ce->data);
}

static bool_t realview_ce_init(struct clockevent_t * ce)
{
	struct realview_clockevent_pdata_t * pdat = (struct realview_clockevent_pdata_t *)ce->priv;

	clk_enable(pdat->clk);
	clockevent_calc_mult_shift(ce, clk_get_rate(pdat->clk), 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	if(!request_irq(pdat->irq, realview_ce_interrupt, IRQ_TYPE_NONE, ce))
	{
		clk_disable(pdat->clk);
		return FALSE;
	}

	write32(pdat->regbase + TIMER_CTRL, 0);
	write32(pdat->regbase + TIMER_LOAD, 0);
	write32(pdat->regbase + TIMER_VALUE, 0);
	write32(pdat->regbase + TIMER_CTRL, (1 << 0) | (1 << 1) | (0 << 2) | (1 << 5) | (0 << 6));
	return TRUE;
}

static bool_t realview_ce_next(struct clockevent_t * ce, u64_t evt)
{
	struct realview_clockevent_pdata_t * pdat = (struct realview_clockevent_pdata_t *)ce->priv;
	write32(pdat->regbase + TIMER_LOAD, (evt & 0xffffffff));
	write32(pdat->regbase + TIMER_VALUE, (evt & 0xffffffff));
	write32(pdat->regbase + TIMER_CTRL, read32(pdat->regbase + TIMER_CTRL) | (1 << 7));
	return TRUE;
}

static __init void realview_clockevent_init(void)
{
	struct realview_clockevent_pdata_t * pdat;
	static struct clockevent_t * ce;

	pdat = malloc(sizeof(struct realview_clockevent_pdata_t));
	if(!pdat)
		return;

	ce = malloc(sizeof(struct clockevent_t));
	if(!ce)
	{
		free(pdat);
		return;
	}

	pdat->clk = "timclk";
	pdat->irq = "TMIER2_3",
	pdat->regbase = phys_to_virt(REALVIEW_TIMER2_BASE);
	ce->name = "realview-ce";
	ce->init = realview_ce_init;
	ce->next = realview_ce_next;
	ce->priv = pdat;

	register_clockevent(ce);
}
core_initcall(realview_clockevent_init);
