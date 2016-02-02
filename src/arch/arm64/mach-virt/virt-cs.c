/*
 * virt-cs.c
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
#include <arm64.h>
#include <clocksource/clocksource.h>

static bool_t cs_init(struct clocksource_t * cs)
{
	clocksource_calc_mult_shift(&cs->mult, &cs->shift, arm64_timer_frequecy(), 1000000000ULL, 10);
	arm64_timer_start();
	return TRUE;
}

static u64_t cs_read(struct clocksource_t * cs)
{
	return arm64_timer_read();
}

static struct clocksource_t cs = {
	.name	= "virt-cs",
	.mask	= CLOCKSOURCE_MASK(64),
	.init	= cs_init,
	.read	= cs_read,
};

static __init void virt_clocksource_init(void)
{
	register_clocksource(&cs);
}
core_initcall(virt_clocksource_init);
