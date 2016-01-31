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

struct clocksource_pdata_t
{
	const char * clk;
	physical_addr_t phys;
	virtual_addr_t virt;
};

static bool_t cs_init(struct clocksource_t * cs)
{
	struct clocksource_pdata_t * pdat = (struct clocksource_pdata_t *)cs->priv;

	clk_enable(pdat->clk);
	cs->mult = clocksource_hz2mult(clk_get_rate(pdat->clk), cs->shift);

	write32(pdat->virt + TIMER_CTRL, 0);
	write32(pdat->virt + TIMER_LOAD, 0xffffffff);
	write32(pdat->virt + TIMER_VALUE, 0xffffffff);
	write32(pdat->virt + TIMER_CTRL, (1 << 1) | (1 << 6) | (1 << 7));
	return TRUE;
}

static u64_t cs_read(struct clocksource_t * cs)
{
	struct clocksource_pdata_t * pdat = (struct clocksource_pdata_t *)cs->priv;
	return (u64_t)(0xffffffff - read32(pdat->virt + TIMER_VALUE));
}

static struct clocksource_pdata_t pdata = {
	.clk	= "timclk",
	.phys	= REALVIEW_TIMER0_BASE,
};

static struct clocksource_t cs = {
	.name	= "realview-cs",
	.shift	= 20,
	.mask	= CLOCKSOURCE_MASK(32),
	.init	= cs_init,
	.read	= cs_read,
	.priv	= &pdata,
};

static __init void realview_clocksource_init(void)
{
	pdata.virt = phys_to_virt(pdata.phys);
	register_clocksource(&cs);
}
core_initcall(realview_clocksource_init);
