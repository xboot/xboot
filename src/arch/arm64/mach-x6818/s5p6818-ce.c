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
#include <s5p6818-timer.h>

#define CE_TIMER_CHANNEL	(0)

static void s5p6818_ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	s5p6818_timer_irq_clear(CE_TIMER_CHANNEL);
	ce->handler(ce, ce->data);
}

static bool_t s5p6818_ce_init(struct clockevent_t * ce)
{
	u64_t rate;

	s5p6818_timer_reset();

	if(!request_irq("TIMER0", s5p6818_ce_interrupt, IRQ_TYPE_NONE, ce))
	{
		LOG("can't request irq 'TIMER0'");
		return FALSE;
	}

	/* 9.375MHZ - 107ns */
	s5p6818_timer_enable(CE_TIMER_CHANNEL, 1);
	rate = s5p6818_timer_calc_tin(CE_TIMER_CHANNEL, 107);
	clockevent_calc_mult_shift(ce, rate, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	s5p6818_timer_count(CE_TIMER_CHANNEL, 0);
	s5p6818_timer_stop(CE_TIMER_CHANNEL);

	return TRUE;
}

static bool_t s5p6818_ce_next(struct clockevent_t * ce, u64_t evt)
{
	s5p6818_timer_count(CE_TIMER_CHANNEL, (evt & 0xffffffff));
	s5p6818_timer_start(CE_TIMER_CHANNEL, 1);
	return TRUE;
}

static struct clockevent_t s5p6818_ce = {
	.name	= "s5p6818-ce",
	.init	= s5p6818_ce_init,
	.next	= s5p6818_ce_next,
};

static __init void s5p6818_clockevent_init(void)
{
	if(register_clockevent(&s5p6818_ce))
		LOG("Register clockevent");
	else
		LOG("Failed to register clockevent");
}
core_initcall(s5p6818_clockevent_init);
