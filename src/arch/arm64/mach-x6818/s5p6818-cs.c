/*
 * s5p6818-cs.c
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
#include <clocksource/clocksource.h>
#include <s5p6818-timer.h>

#define CS_TIMER_CHANNEL	(1)

static bool_t s5p6818_cs_init(struct clocksource_t * cs)
{
	u64_t rate;

	s5p6818_timer_reset();

	/* 75MHZ - 13.333...ns */
	s5p6818_timer_enable(CS_TIMER_CHANNEL, 0);
	rate = s5p6818_timer_calc_tin(CS_TIMER_CHANNEL, 13);
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, rate, 1000000000ULL, 10);
	s5p6818_timer_count(CS_TIMER_CHANNEL, 0xffffffff);
	s5p6818_timer_start(CS_TIMER_CHANNEL, 0);

	return TRUE;
}

static u64_t s5p6818_cs_read(struct clocksource_t * cs)
{
	u32_t val = s5p6818_timer_read(CS_TIMER_CHANNEL);
	return (u64_t)(~val);
}

static struct clocksource_t s5p6818_cs = {
	.name	= "s5p6818-cs",
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= s5p6818_cs_init,
	.read	= s5p6818_cs_read,
};

static __init void s5p6818_clocksource_init(void)
{
	if(register_clocksource(&s5p6818_cs))
		LOG("Register clocksource");
	else
		LOG("Failed to register clocksource");
}
core_initcall(s5p6818_clocksource_init);
