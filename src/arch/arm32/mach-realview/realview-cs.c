/*
 * realview-cs.c
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
#include <realview/reg-timer.h>

static void realview_cs_init(struct clocksource_t * cs)
{
	u64_t rate;

	clk_enable("timclk");
	rate = clk_get_rate("timclk");
	cs->mult = clocksource_hz2mult(rate, cs->shift);

	writel(REALVIEW_T0_CTRL, 0);
	writel(REALVIEW_T0_LOAD, 0xffffffff);
	writel(REALVIEW_T0_VALUE, 0xffffffff);
	writel(REALVIEW_T0_CTRL, REALVIEW_TC_32BIT | REALVIEW_TC_ENABLE | REALVIEW_TC_PERIODIC);
}

static cycle_t realview_cs_read(struct clocksource_t * cs)
{
	u32_t val = readl(REALVIEW_T0_VALUE);
	return (cycle_t)(~val);
}

static struct clocksource_t realview_cs = {
	.name	= "realview-cs",
	.shift	= 20,
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= realview_cs_init,
	.read	= realview_cs_read,
};

static __init void realview_clocksource_init(void)
{
	if(register_clocksource(&realview_cs))
		LOG("Register clocksource");
	else
		LOG("Failed to register clocksource");
}
core_initcall(realview_clocksource_init);
