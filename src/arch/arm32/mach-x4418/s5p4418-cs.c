/*
 * s5p4418-cs.c
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
#include <clocksource/clocksource.h>
#include <s5p4418-timer.h>

#define CS_TIMER_CHANNEL	(0)

static void s5p4418_cs_init(struct clocksource_t * cs)
{
	u64_t rate;

	s5p4418_timer_reset();

	/* 40ns ~ 25MHZ */
	rate = s5p4418_timer_calc_tin(CS_TIMER_CHANNEL, 40);
	s5p4418_timer_stop(CS_TIMER_CHANNEL, 0);
	s5p4418_timer_count(CS_TIMER_CHANNEL, 0xffffffff);
	s5p4418_timer_start(CS_TIMER_CHANNEL, 0);
	cs->mult = clocksource_hz2mult(rate, cs->shift);
}

static cycle_t s5p4418_cs_read(struct clocksource_t * cs)
{
	u32_t val = s5p4418_timer_read(CS_TIMER_CHANNEL);
	return (cycle_t)(~val);
}

static struct clocksource_t s5p4418_cs = {
	.name	= "s5p4418-cs",
	.shift	= 20,
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= s5p4418_cs_init,
	.read	= s5p4418_cs_read,
};

static __init void s5p4418_clocksource_init(void)
{
	if(register_clocksource(&s5p4418_cs))
		LOG("Register clocksource");
	else
		LOG("Failed to register clocksource");
}
core_initcall(s5p4418_clocksource_init);
