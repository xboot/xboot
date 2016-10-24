/*
 * driver/clk-rk3399-pll.c
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
#include <rk3399/reg-cru.h>
#include <rk3399/reg-pmu-cru.h>

struct clk_rk3399_pll_pdata_t {
	virtual_addr_t cru;
	virtual_addr_t pmucru;
	char * parent;
	int channel;
};

static void clk_rk3399_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_rk3399_pll_get_parent(struct clk_t * clk)
{
	struct clk_rk3399_pll_pdata_t * pdat = (struct clk_rk3399_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_rk3399_pll_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_rk3399_pll_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_rk3399_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_rk3399_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_rk3399_pll_pdata_t * pdat = (struct clk_rk3399_pll_pdata_t *)clk->priv;
	u32_t con, pll_con0, pll_con1, pll_con2, dsmp;
	u64_t rate = 0, frac_rate = 0;

	switch(pdat->channel)
	{
	case 0 ... 6:
		con = (read32(pdat->cru + CRU_PLL_CON(pdat->channel, 3)) >> 8) & 0x3;
		pll_con0 = read32(pdat->cru + CRU_PLL_CON(pdat->channel, 0));
		pll_con1 = read32(pdat->cru + CRU_PLL_CON(pdat->channel, 1));
		pll_con2 = read32(pdat->cru + CRU_PLL_CON(pdat->channel, 2));
		dsmp = (read32(pdat->cru + CRU_PLL_CON(pdat->channel, 3)) >> 3) & 0x1;
		break;

	case 7:
		con = (read32(pdat->pmucru + PMU_CRU_PLL_CON(0, 3)) >> 8) & 0x3;
		pll_con0 = read32(pdat->pmucru + PMU_CRU_PLL_CON(0, 0));
		pll_con1 = read32(pdat->pmucru + PMU_CRU_PLL_CON(0, 1));
		pll_con2 = read32(pdat->pmucru + PMU_CRU_PLL_CON(0, 2));
		dsmp = (read32(pdat->pmucru + PMU_CRU_PLL_CON(0, 3)) >> 3) & 0x1;
		break;

	default:
		return 0;
	}

	if(con == 0x1)
	{
		rate = (u64_t)(24000000) * ((pll_con0 >> 0) & 0xfff);
		rate = rate / ((pll_con1 >> 0) & 0x3f);
		if(dsmp == 0)
		{
			frac_rate = (u64_t)(24000000) * ((pll_con2 >> 0) & 0xffffff);
			frac_rate = frac_rate / ((pll_con1 >> 0) & 0x3f);
			rate += frac_rate >> 24;
		}
		rate = rate / ((pll_con1 >> 8) & 0x7);
		rate = rate / ((pll_con1 >> 12) & 0x7);

		return rate;
	}
	else if(con == 0x2)
	{
		rate = 32768;
	}
	else
	{
		rate = 24000000;
	}

	return rate;
}

static struct device_t * clk_rk3399_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rk3399_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 7)
		return NULL;

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rk3399_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->cru = phys_to_virt(RK3399_CRU_BASE);
	pdat->pmucru = phys_to_virt(RK3399_PMU_CRU_BASE);
	pdat->parent = strdup(parent);
	pdat->channel = channel;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rk3399_pll_set_parent;
	clk->get_parent = clk_rk3399_pll_get_parent;
	clk->set_enable = clk_rk3399_pll_set_enable;
	clk->get_enable = clk_rk3399_pll_get_enable;
	clk->set_rate = clk_rk3399_pll_set_rate;
	clk->get_rate = clk_rk3399_pll_get_rate;
	clk->priv = pdat;

	if(!register_clk(&dev, clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void clk_rk3399_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rk3399_pll_pdata_t * pdat = (struct clk_rk3399_pll_pdata_t *)clk->priv;

	if(clk && unregister_clk(clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rk3399_pll_suspend(struct device_t * dev)
{
}

static void clk_rk3399_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_rk3399_pll = {
	.name		= "clk-rk3399-pll",
	.probe		= clk_rk3399_pll_probe,
	.remove		= clk_rk3399_pll_remove,
	.suspend	= clk_rk3399_pll_suspend,
	.resume		= clk_rk3399_pll_resume,
};

static __init void clk_rk3399_pll_driver_init(void)
{
	register_driver(&clk_rk3399_pll);
}

static __exit void clk_rk3399_pll_driver_exit(void)
{
	unregister_driver(&clk_rk3399_pll);
}

driver_initcall(clk_rk3399_pll_driver_init);
driver_exitcall(clk_rk3399_pll_driver_exit);
