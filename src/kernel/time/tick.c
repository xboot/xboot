/*
 * kernel/time/tick.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <time/tick.h>

inline void tick_interrupt(void)
{
	jiffies++;

	extern void exec_timer_task(void);
	exec_timer_task();
}

volatile u32_t jiffies = 0;
EXPORT_SYMBOL(jiffies);

volatile u32_t HZ = 0;
EXPORT_SYMBOL(HZ);

u64_t jiffies_to_msecs(const u64_t j)
{
	if(HZ > 0)
		return ((u64_t)j * 1000L) / HZ;
	return 0;
}
EXPORT_SYMBOL(jiffies_to_msecs);

u64_t jiffies_to_usecs(const u64_t j)
{
	if(HZ > 0)
		return ((u64_t)j * 1000000L) / HZ;
	return 0;
}
EXPORT_SYMBOL(jiffies_to_usecs);

u64_t msecs_to_jiffies(const u64_t m)
{
	return (u64_t)m * HZ / 1000L;
}
EXPORT_SYMBOL(msecs_to_jiffies);

u64_t usecs_to_jiffies(const u64_t u)
{
	return (u64_t)u * HZ / 1000000L;
}
EXPORT_SYMBOL(usecs_to_jiffies);

static struct tick_t * __tick = NULL;
bool_t register_tick(struct tick_t * tick)
{
	if(!tick || (tick->hz <= 0) || (!tick->init))
		return FALSE;

	__tick = tick;
	return TRUE;
}

static void __cs_tick_init(struct clocksource_t * cs)
{
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, HZ, 1000000LL, 10);
}

static cycle_t __cs_tick_read(struct clocksource_t * cs)
{
	return (cycle_t)(jiffies);
}

static struct clocksource_t __cs_tick = {
	.name	= "tick",
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= __cs_tick_init,
	.read	= __cs_tick_read,
};

void subsys_init_tick(void)
{
	if(!__tick || !__tick->init())
	{
		LOG("No system tick");
		return;
	}

	jiffies = 0;
	HZ = __tick->hz;

	register_clocksource(&__cs_tick);
	LOG("Initial system tick");
}
