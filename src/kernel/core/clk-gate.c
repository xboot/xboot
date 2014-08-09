/*
 * kernel/core/clk-gate.c
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

#include <xboot/clk.h>

static void clk_gate_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_gate_get_parent(struct clk_t * clk)
{
	struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;
	return gclk->parent;
}

static void clk_gate_set_enable(struct clk_t * clk, bool_t enable)
{
	struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;

	if(enable)
		writel(gclk->reg, (readl(gclk->reg) & ~(0x1 << gclk->shift)) | ((gclk->invert ? 0x0 : 0x1) << gclk->shift));
	else
		writel(gclk->reg, (readl(gclk->reg) & ~(0x1 << gclk->shift)) | ((gclk->invert ? 0x1 : 0x0) << gclk->shift));
}

static bool_t clk_gate_get_enable(struct clk_t * clk)
{
	struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;

	if(readl(gclk->reg) & (0x1 << gclk->shift))
		return gclk->invert ? FALSE : TRUE;
	return gclk->invert ? TRUE : FALSE;
}

static void clk_gate_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_gate_get_rate(struct clk_t * clk, u64_t prate)
{
	return prate;
}

bool_t clk_gate_register(struct clk_gate_t * gclk)
{
	struct clk_t * clk;

	if(!gclk || !gclk->name)
		return FALSE;

	if(clk_search(gclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = gclk->name;
	clk->set_parent = clk_gate_set_parent;
	clk->get_parent = clk_gate_get_parent;
	clk->set_enable = clk_gate_set_enable;
	clk->get_enable = clk_gate_get_enable;
	clk->set_rate = clk_gate_set_rate;
	clk->get_rate = clk_gate_get_rate;
	clk->priv = gclk;

	if(!clk_register(clk))
	{
		free(clk);
		return FALSE;
	}

	return TRUE;
}

bool_t clk_gate_unregister(struct clk_gate_t * gclk)
{
	struct clk_t * clk;

	if(!gclk || !gclk->name)
		return FALSE;

	clk = clk_search(gclk->name);
	if(!clk)
		return FALSE;

	if(clk_unregister(clk))
	{
		free(clk);
		return TRUE;
	}

	return FALSE;
}
