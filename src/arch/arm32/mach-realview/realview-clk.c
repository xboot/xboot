/*
 * realview-clk.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <sizes.h>
#include <types.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>


/*
 * the array of clocks, which will to be setup.
 */
static struct clk realview_clocks[6];

/*
 * setup the realview's clock array.
 */
static void realview_setup_clocks(u64_t xtal)
{
	/* initialize system clocks */
	realview_clocks[0].name = "xtal";
	realview_clocks[0].rate = xtal;

	/* uart clock */
	realview_clocks[1].name = "uclk";
	realview_clocks[1].rate = 24*1000*1000;

	/* kmi clock */
	realview_clocks[2].name = "kclk";
	realview_clocks[2].rate = 24*1000*1000;

	/* mmci clock */
	realview_clocks[3].name = "mclk";
	realview_clocks[3].rate = 24*1000*1000;

	/* timer clock */
	realview_clocks[4].name = "timclk";
	realview_clocks[4].rate = 1*1000*1000;

	/* ref clock */
	realview_clocks[5].name = "refclk";
	realview_clocks[5].rate = 32768;
}

static __init void realview_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	/* get system xtal */
	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 12*1000*1000;

	/* setup clock arrays */
	realview_setup_clocks(xtal);

	/* register clocks to system */
	for(i=0; i< ARRAY_SIZE(realview_clocks); i++)
	{
		if(!clk_register(&realview_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", realview_clocks[i].name);
		}
	}
}

static __exit void realview_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(realview_clocks); i++)
	{
		if(!clk_unregister(&realview_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", realview_clocks[i].name);
		}
	}
}

core_initcall(realview_clk_init);
core_exitcall(realview_clk_exit);
