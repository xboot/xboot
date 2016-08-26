/*
 * sandbox-ce.c
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
#include <sandbox.h>

static void ce_callback(void * data)
{
	struct clockevent_t * ce = (struct clockevent_t *)data;
	ce->handler(ce, ce->data);
}

static bool_t ce_init(struct clockevent_t * ce)
{
	/* 1KHZ - 1ms */
	clockevent_calc_mult_shift(ce, 1000, 10);
	ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
	ce->max_delta_ns = clockevent_delta2ns(ce, 0x7fffffff);
	sandbox_sdl_timer_init();

	return TRUE;
}

static bool_t ce_next(struct clockevent_t * ce, u64_t evt)
{
	sandbox_sdl_timer_set_next((evt & 0x7fffffff), ce_callback, ce);
	return TRUE;
}

static struct clockevent_t ce = {
	.name	= "sandbox-ce",
	.init	= ce_init,
	.next	= ce_next,
};

static __init void sandbox_clockevent_init(void)
{
	register_clockevent(&ce);
}
core_initcall(sandbox_clockevent_init);
