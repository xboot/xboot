/*
 * virt-ce.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <arm64.h>
#include <clockevent/clockevent.h>

static void virt_ce_interrupt(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	ce->handler(ce, ce->data);
}

static bool_t virt_ce_init(struct clockevent_t * ce)
{
	u64_t rate = arm64_timer_frequecy();

	if(!request_irq("NSPHYS_TIMER", virt_ce_interrupt, IRQ_TYPE_NONE, ce))
		return FALSE;

	clockevent_calc_mult_shift(ce, rate, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
	arm64_timer_start(0);

	return TRUE;
}

static bool_t virt_ce_next(struct clockevent_t * ce, u64_t evt)
{
	arm64_timer_compare(evt);
	return TRUE;
}

static struct clockevent_t virt_ce = {
	.name	= "virt-ce",
	.init	= virt_ce_init,
	.next	= virt_ce_next,
};

static __init void virt_clockevent_init(void)
{
	if(register_clockevent(&virt_ce))
		LOG("Register clockevent");
	else
		LOG("Failed to register clockevent");
}
core_initcall(virt_clockevent_init);
