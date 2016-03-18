/*
 * kernel/time/keeper.c
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

#include <clocksource/clocksource.h>
#include <time/keeper.h>

struct keeper_t {
	struct clocksource_t * cs;
	struct timer_t timer;
	seqlock_t lock;
	u64_t interval;
	u64_t last;
	u64_t nsec;
};
static struct keeper_t __keeper = { 0 };

static int keeper_timer_function(struct timer_t * timer, void * data)
{
	struct keeper_t * keeper = (struct keeper_t *)(data);
	struct clocksource_t * cs = keeper->cs;
	u64_t now, delta, offset;
	irq_flags_t flags;

	write_seqlock_irqsave(&keeper->lock, flags);
	now = clocksource_cycle(cs);
	delta = clocksource_delta(cs, keeper->last, now);
	offset = clocksource_delta2ns(cs, delta);
	keeper->nsec += offset;
	keeper->last = now;
	write_sequnlock_irqrestore(&keeper->lock, flags);

	timer_forward_now(timer, ns_to_ktime(keeper->interval));
	return 1;
}

ktime_t ktime_get(void)
{
	struct keeper_t * keeper = &__keeper;
	struct clocksource_t * cs = keeper->cs ? keeper->cs : clocksource_dummy();
	u64_t now, delta, offset;
	unsigned int seq;

	do {
		seq = read_seqbegin(&keeper->lock);
		now = clocksource_cycle(cs);
		delta = clocksource_delta(cs, keeper->last, now);
		offset = clocksource_delta2ns(cs, delta);
	} while(read_seqretry(&keeper->lock, seq));

	return ns_to_ktime(keeper->nsec + offset);
}

void subsys_init_keeper(void)
{
	struct keeper_t * keeper = &__keeper;
	struct clocksource_t * cs = clocksource_best();

	keeper->cs = cs;
	keeper->interval = clocksource_deferment(cs) >> 1;
	keeper->last = clocksource_cycle(cs);
	keeper->nsec = 0;
	seqlock_init(&keeper->lock);
	timer_init(&keeper->timer, keeper_timer_function, keeper);
	timer_start_now(&keeper->timer, ns_to_ktime(keeper->interval));
}
