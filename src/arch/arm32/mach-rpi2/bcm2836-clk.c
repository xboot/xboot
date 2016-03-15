/*
 * bcm2836-clk.c
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
#include <bcm2836-mbox.h>

struct clk_mbox_t {
	const char * name;
	int id;
};

static void clk_mbox_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_mbox_get_parent(struct clk_t * clk)
{
	return NULL;
}

static void clk_mbox_set_enable(struct clk_t * clk, bool_t enable)
{
	struct clk_mbox_t * mclk = (struct clk_mbox_t *)clk->priv;
	bcm2836_mbox_clock_set_state(mclk->id, enable ? 0x1 : 0x0);
}

static bool_t clk_mbox_get_enable(struct clk_t * clk)
{
	struct clk_mbox_t * mclk = (struct clk_mbox_t *)clk->priv;
	return (bcm2836_mbox_clock_get_state(mclk->id) == 0x1) ? TRUE : FALSE;
}

static void clk_mbox_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_mbox_t * mclk = (struct clk_mbox_t *)clk->priv;
	bcm2836_mbox_clock_set_rate(mclk->id, rate);
}

static u64_t clk_mbox_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_mbox_t * mclk = (struct clk_mbox_t *)clk->priv;
	return bcm2836_mbox_clock_get_rate(mclk->id);
}

static bool_t clk_mbox_register(struct clk_mbox_t * mclk)
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
	clk->type = CLK_TYPE_OTHER;
	clk->count = 0;
	clk->set_parent = clk_mbox_set_parent;
	clk->get_parent = clk_mbox_get_parent;
	clk->set_enable = clk_mbox_set_enable;
	clk->get_enable = clk_mbox_get_enable;
	clk->set_rate = clk_mbox_set_rate;
	clk->get_rate = clk_mbox_get_rate;
	clk->priv = mclk;

	if(!clk_register(clk))
	{
		free(clk);
		return FALSE;
	}

	return TRUE;
}

static struct clk_mbox_t bcm2836_mbox_clks[] = {
	{
		.name = "emmc-clk",
		.id = MBOX_CLOCK_ID_EMMC,
	}, {
		.name = "uart-clk",
		.id = MBOX_CLOCK_ID_UART,
	}, {
		.name = "arm-clk",
		.id = MBOX_CLOCK_ID_ARM,
	}, {
		.name = "core-clk",
		.id = MBOX_CLOCK_ID_CORE,
	}, {
		.name = "v3d-clk",
		.id = MBOX_CLOCK_ID_V3D,
	}, {
		.name = "h264-clk",
		.id = MBOX_CLOCK_ID_H264,
	}, {
		.name = "isp-clk",
		.id = MBOX_CLOCK_ID_ISP,
	}, {
		.name = "sdram-clk",
		.id = MBOX_CLOCK_ID_SDRAM,
	}, {
		.name = "pixel-clk",
		.id = MBOX_CLOCK_ID_PIXEL,
	}, {
		.name = "pwm-clk",
		.id = MBOX_CLOCK_ID_PWM,
	}
};

static struct clk_fixed_t bcm2836_fixed_clks[] = {
	{
		.name = "osc-clk",
		.rate = 19200000,
	}, {
		.name = "sys-timer-clk",
		.rate = 1 * 1000 * 1000,
	}, {
		.name = "arm-timer-clk",
		.rate = 250 * 1000 * 1000,
	}
};

static __init void bcm2836_clk_init(void)
{
	int i;

	bcm2836_mbox_clock_set_rate(MBOX_CLOCK_ID_PWM, 9200000);
	bcm2836_mbox_clock_set_rate(MBOX_CLOCK_ID_UART, 24 * 1000 * 1000);

	for(i = 0; i < ARRAY_SIZE(bcm2836_mbox_clks); i++)
		clk_mbox_register(&bcm2836_mbox_clks[i]);

	for(i = 0; i < ARRAY_SIZE(bcm2836_fixed_clks); i++)
		clk_fixed_register(&bcm2836_fixed_clks[i]);
}
core_initcall(bcm2836_clk_init);
