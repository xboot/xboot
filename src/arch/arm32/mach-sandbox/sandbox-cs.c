/*
 * sandbox-cs.c
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
#include <sandbox.h>

static bool_t sandbox_cs_init(struct clocksource_t * cs)
{
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, sandbox_get_time_frequency(), 1000000000ULL, 10);
	return TRUE;
}

static u64_t sandbox_cs_read(struct clocksource_t * cs)
{
	return sandbox_get_time_counter();
}

static struct clocksource_t sandbox_cs = {
	.name	= "sandbox-cs",
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= sandbox_cs_init,
	.read	= sandbox_cs_read,
};

static __init void sandbox_clocksource_init(void)
{
	if(register_clocksource(&sandbox_cs))
		LOG("Register clocksource");
	else
		LOG("Failed to register clocksource");
}
core_initcall(sandbox_clocksource_init);
