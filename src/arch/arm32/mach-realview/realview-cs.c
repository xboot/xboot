/*
 * realview-cs.c
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
#include <realview/reg-timer.h>

struct realview_clocksource_pdata_t
{
	char * clk;
	virtual_addr_t regbase;
};

static bool_t realview_cs_init(struct clocksource_t * cs)
{
	struct realview_clocksource_pdata_t * dat = (struct realview_clocksource_pdata_t *)cs->priv;

	clk_enable(dat->clk);
	cs->mult = clocksource_hz2mult(clk_get_rate(dat->clk), cs->shift);
	write32(dat->regbase + TIMER_CTRL, 0);
	write32(dat->regbase + TIMER_LOAD, 0xffffffff);
	write32(dat->regbase + TIMER_VALUE, 0xffffffff);
	write32(dat->regbase + TIMER_CTRL, (1 << 1) | (1 << 6) | (1 << 7));
	return TRUE;
}

static u64_t realview_cs_read(struct clocksource_t * cs)
{
	struct realview_clocksource_pdata_t * dat = (struct realview_clocksource_pdata_t *)cs->priv;
	u32_t val = read32(dat->regbase + TIMER_VALUE);
	return (u64_t)(~val);
}

static __init void realview_clocksource_init(void)
{
	struct realview_clocksource_pdata_t * dat;
	static struct clocksource_t * cs;

	dat = malloc(sizeof(struct realview_clocksource_pdata_t));
	if(!dat)
		return;

	cs = malloc(sizeof(struct clocksource_t));
	if(!cs)
	{
		free(dat);
		return;
	}

	dat->clk = "timclk";
	dat->regbase = phys_to_virt(REALVIEW_TIMER0_BASE);
	cs->name = "realview-cs";
	cs->shift = 20;
	cs->mask = CLOCKSOURCE_MASK(32);
	cs->init = realview_cs_init;
	cs->read = realview_cs_read;
	cs->priv = dat;

	register_clocksource(cs);
}
core_initcall(realview_clocksource_init);
