/*
 * driver/clk-mux.c
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

#include <clk/clk.h>

static void clk_mux_set_parent(struct clk_t * clk, const char * pname)
{
	struct clk_mux_t * mclk = (struct clk_mux_t *)clk->priv;
	struct clk_mux_table_t * table = mclk->parent;
	u32_t val;

	for(table = mclk->parent; table && table->name; table++)
	{
		if(strcmp(table->name, pname) == 0)
		{
			val = readl(mclk->reg);
			val &= ~(((1 << mclk->width) - 1) << mclk->shift);
			val |= table->val << mclk->shift;
			writel(mclk->reg, val);
			return;
		}
	}
}

static const char * clk_mux_get_parent(struct clk_t * clk)
{
	struct clk_mux_t * mclk = (struct clk_mux_t *)clk->priv;
	struct clk_mux_table_t * table = mclk->parent;
	int val = readl(mclk->reg) >> mclk->shift & ((1 << mclk->width) - 1);

	for(table = mclk->parent; table && table->name; table++)
	{
		if(table->val == val)
			return table->name;
	}
	return NULL;
}

static void clk_mux_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_mux_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_mux_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_mux_get_rate(struct clk_t * clk, u64_t prate)
{
	return prate;
}

bool_t clk_mux_register(struct clk_mux_t * mclk)
{
	struct clk_t * clk;

	if(!mclk || !mclk->name)
		return FALSE;

	if(clk_search(mclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = mclk->name;
	clk->set_parent = clk_mux_set_parent;
	clk->get_parent = clk_mux_get_parent;
	clk->set_enable = clk_mux_set_enable;
	clk->get_enable = clk_mux_get_enable;
	clk->set_rate = clk_mux_set_rate;
	clk->get_rate = clk_mux_get_rate;
	clk->priv = mclk;

	if(!clk_register(clk))
	{
		free(clk);
		return FALSE;
	}

	return TRUE;
}

bool_t clk_mux_unregister(struct clk_mux_t * mclk)
{
	struct clk_t * clk;

	if(!mclk || !mclk->name)
		return FALSE;

	clk = clk_search(mclk->name);
	if(!clk)
		return FALSE;

	if(clk_unregister(clk))
	{
		free(clk);
		return TRUE;
	}

	return FALSE;
}
