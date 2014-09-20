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

static struct tick_t * __tick = NULL;

volatile u32_t jiffies = 0;
EXPORT_SYMBOL(jiffies);

volatile u32_t HZ = 0;
EXPORT_SYMBOL(HZ);

inline void tick_interrupt(void)
{
	jiffies++;

	extern void exec_timer_task(void);
	exec_timer_task();
}

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

bool_t register_tick(struct tick_t * tick)
{
	if( tick && (tick->hz > 0) && (tick->init != NULL))
	{
		__tick = tick;
		return TRUE;
	}
	else
	{
		__tick = NULL;
		return FALSE;
	}
}

static void __cs_tick_init(struct clocksource_t * cs)
{
	clocks_calc_mult_shift(&cs->mult, &cs->shift, HZ, 1000000L, 10);
}

static u64_t __cs_tick_read(struct clocksource_t * cs)
{
	return (u64_t)(jiffies);
}

static struct clocksource_t __cs_tick = {
	.name	= "tick-cs",
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= __cs_tick_init,
	.read	= __cs_tick_read,
};

static void tick_clocksource_init(void)
{
	if(register_clocksource(&__cs_tick))
		LOG("Register clocksource");
	else
		LOG("Failed to register clocksource");
}

bool_t init_system_tick(void)
{
	HZ = 0;
	jiffies = 0;

	if(!__tick)
		return FALSE;

	if(!__tick->init())
		return FALSE;

	HZ = __tick->hz;
	tick_clocksource_init();

	return TRUE;
}
