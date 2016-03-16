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
	u64_t delta;
	u64_t last;
	u64_t nsec;
};
static struct keeper_t __keeper;

static int timer_keeper_function(struct timer_t * timer, void * data)
{
	struct keeper_t * keeper = (struct keeper_t *)(data);
	struct clocksource_t * cs = keeper->cs;
	u64_t now, delta;
	u64_t offset;

	now = clocksource_read(cs);
	if(keeper->last > now)
		delta = (cs->mask - keeper->last + now + 1) & cs->mask;
	else
		delta = (now - keeper->last) & cs->mask;
	offset = ((u64_t)delta * cs->mult) >> cs->shift;
	keeper->nsec += offset;
	keeper->last = now;

	timer_forward_now(timer, ns_to_ktime(keeper->delta));
	return 1;
}

ktime_t ktime_get(void)
{
	struct keeper_t * keeper = &__keeper;
	struct clocksource_t * cs = keeper->cs;
	u64_t now, delta;
	u64_t offset;

	now = clocksource_read(cs);
	if(keeper->last > now)
		delta = (cs->mask - keeper->last + now + 1) & cs->mask;
	else
		delta = (now - keeper->last) & cs->mask;
	offset = ((u64_t)delta * cs->mult) >> cs->shift;
	return ns_to_ktime(keeper->nsec + offset);
}

void subsys_init_keeper(void)
{
	struct keeper_t * keeper = &__keeper;
	struct clocksource_t * cs = clocksource_get_best();
	u64_t delta = ((cs->mask * cs->mult) >> cs->shift) * 3 / 4;

	keeper->cs = cs;
	keeper->delta = delta;
	keeper->last = clocksource_read(cs);
	keeper->nsec = 0;
	timer_init(&keeper->timer, timer_keeper_function, keeper);
	timer_start_now(&keeper->timer, ns_to_ktime(keeper->delta));
}
