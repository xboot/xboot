/*
 * driver/clk/clk-pll.c
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

#include <clk/clk.h>

static void clk_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_pll_get_parent(struct clk_t * clk)
{
	struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
	return pclk->parent;
}

static void clk_pll_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_pll_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
	if(pclk->set_rate)
		pclk->set_rate(pclk, prate, rate);
}

static u64_t clk_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
	if(pclk->get_rate)
		return pclk->get_rate(pclk, prate);
	return 0;
}

bool_t register_clk_pll(struct device_t ** device, struct clk_pll_t * pclk)
{
	struct clk_t * clk;

	if(!pclk || !pclk->name)
		return FALSE;

	if(search_clk(pclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = pclk->name;
	clk->type = CLK_TYPE_PLL;
	clk->count = 0;
	clk->set_parent = clk_pll_set_parent;
	clk->get_parent = clk_pll_get_parent;
	clk->set_enable = clk_pll_set_enable;
	clk->get_enable = clk_pll_get_enable;
	clk->set_rate = clk_pll_set_rate;
	clk->get_rate = clk_pll_get_rate;
	clk->priv = pclk;

	if(!register_clk(device, clk))
	{
		free(clk);
		return FALSE;
	}
	return TRUE;
}

bool_t unregister_clk_pll(struct clk_pll_t * pclk)
{
	struct clk_t * clk;

	if(!pclk || !pclk->name)
		return FALSE;

	clk = search_clk(pclk->name);
	if(!clk)
		return FALSE;

	if(unregister_clk(clk))
	{
		free(clk);
		return TRUE;
	}
	return FALSE;
}
