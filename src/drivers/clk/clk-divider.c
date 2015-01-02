/*
 * driver/clk/clk-divider.c
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

#include <clk/clk.h>

#define div_mask(d)		((1 << (d->width)) - 1)

static void clk_divider_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_divider_get_parent(struct clk_t * clk)
{
	struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
	return dclk->parent;
}

static void clk_divider_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_divider_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_divider_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
	u32_t div = prate / rate;
	u32_t val;

	if(dclk->type == CLK_DIVIDER_ONE_BASED)
		div--;

	if(div > div_mask(dclk))
		div = div_mask(dclk);

	val = readl(dclk->reg);
	val &= ~(div_mask(dclk) << dclk->shift);
	val |= div << dclk->shift;
	writel(dclk->reg, val);
}

static u64_t clk_divider_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
	u32_t div;

	div = readl(dclk->reg) >> dclk->shift;
	div &= div_mask(dclk);

	if(dclk->type == CLK_DIVIDER_ONE_BASED)
		div++;

	return prate / div;
}

bool_t clk_divider_register(struct clk_divider_t * dclk)
{
	struct clk_t * clk;

	if(!dclk || !dclk->name)
		return FALSE;

	if(clk_search(dclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = dclk->name;
	clk->type = CLK_TYPE_DIVIDER;
	clk->count = 0;
	clk->set_parent = clk_divider_set_parent;
	clk->get_parent = clk_divider_get_parent;
	clk->set_enable = clk_divider_set_enable;
	clk->get_enable = clk_divider_get_enable;
	clk->set_rate = clk_divider_set_rate;
	clk->get_rate = clk_divider_get_rate;
	clk->priv = dclk;

	if(!clk_register(clk))
	{
		free(clk);
		return FALSE;
	}

	return TRUE;
}

bool_t clk_divider_unregister(struct clk_divider_t * dclk)
{
	struct clk_t * clk;

	if(!dclk || !dclk->name)
		return FALSE;

	clk = clk_search(dclk->name);
	if(!clk)
		return FALSE;

	if(clk_unregister(clk))
	{
		free(clk);
		return TRUE;
	}

	return FALSE;
}
