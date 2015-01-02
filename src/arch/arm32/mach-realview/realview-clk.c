/*
 * realview-clk.c
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

static struct clk_fixed_t realview_clks[] = {
	{
		.name = "xtal",
		.rate = 12 * 1000 * 1000,
	}, {
		.name = "uclk",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "kclk",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "mclk",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "timclk",
		.rate = 1 * 1000 * 1000,
	}, {
		.name = "refclk",
		.rate = 32768,
	}
};

static __init void realview_clk_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(realview_clks); i++)
	{
		if(clk_fixed_register(&realview_clks[i]))
			LOG("Register clock source '%s' [%LdHZ]", realview_clks[i].name, realview_clks[i].rate);
		else
			LOG("Failed to register clock source '%s' [%LdHZ]", realview_clks[i].name, realview_clks[i].rate);
	}
}

static __exit void realview_clk_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(realview_clks); i++)
	{
		if(clk_fixed_unregister(&realview_clks[i]))
			LOG("Unregister clock source '%s' [%LdHZ]", realview_clks[i].name, realview_clks[i].rate);
		else
			LOG("Failed to unregister clock '%s' [%LdHZ]", realview_clks[i].name, realview_clks[i].rate);
	}
}

core_initcall(realview_clk_init);
core_exitcall(realview_clk_exit);
