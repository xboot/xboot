/*
 * s5p4418-tick.c
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
#include <s5p4418-timer.h>

#define TICK_TIMER_CHANNEL		(0)

static void timer_interrupt(void * data)
{
	tick_interrupt();
	s5p4418_timer_irq_clear(TICK_TIMER_CHANNEL);
}

static bool_t tick_timer_init(void)
{
	u64_t rate;

	s5p4418_timer_reset();

	if(!request_irq("TIMER0", timer_interrupt, IRQ_TYPE_NONE, NULL))
	{
		LOG("can't request irq 'TIMER0'");
		return FALSE;
	}

	/* 75MHZ - 13.333...ns */
	rate = s5p4418_timer_calc_tin(TICK_TIMER_CHANNEL, 13);
	s5p4418_timer_stop(TICK_TIMER_CHANNEL, 1);
	s5p4418_timer_count(TICK_TIMER_CHANNEL, rate / 100);
	s5p4418_timer_start(TICK_TIMER_CHANNEL, 1);

	return TRUE;
}

static struct tick_t s5p4418_tick = {
	.hz		= 100,
	.init	= tick_timer_init,
};

static __init void s5p4418_tick_init(void)
{
	if(register_tick(&s5p4418_tick))
		LOG("Register tick");
	else
		LOG("Failed to register tick");
}
core_initcall(s5p4418_tick_init);
