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

static void realview_ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_ICLR), 0x0);
	ce->handler(ce, ce->data);
}

static bool_t realview_ce_init(struct clockevent_t * ce)
{
	u64_t rate;

	clk_enable("timclk");
	rate = clk_get_rate("timclk");

	if(!request_irq("TMIER2_3", realview_ce_interrupt, IRQ_TYPE_NONE, ce))
	{
		LOG("Can't request irq 'TMIER2_3'");
		clk_disable("timclk");
		return FALSE;
	}

	clockevent_calc_mult_shift(ce, rate, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_CTRL), 0);
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_LOAD), 0);
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_VALUE), 0);
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_CTRL), (1 << 0) | (1 << 1) | (0 << 2) | (1 << 5) | (0 << 6));

	return TRUE;
}

static bool_t realview_ce_next(struct clockevent_t * ce, u64_t evt)
{
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_LOAD), (evt & 0xffffffff));
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_VALUE), (evt & 0xffffffff));
	write32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_CTRL), read32(phys_to_virt(REALVIEW_TIMER3_BASE + TIMER_CTRL)) | (1 << 7));
	return TRUE;
}

static struct clockevent_t realview_ce = {
	.name	= "realview-ce",
	.init	= realview_ce_init,
	.next	= realview_ce_next,
};

static __init void realview_clockevent_init(void)
{
	register_clockevent(&realview_ce);
}
core_initcall(realview_clockevent_init);
