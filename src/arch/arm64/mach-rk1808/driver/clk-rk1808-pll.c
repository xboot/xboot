/*
 * driver/clk-rk1808-pll.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <clk/clk.h>
#include <rk1808/reg-cru.h>
#include <rk1808/reg-pmu-cru.h>

struct clk_rk1808_pll_pdata_t {
	virtual_addr_t cru;
	virtual_addr_t pmucru;
	char * parent;
	int channel;
};

static void clk_rk1808_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_rk1808_pll_get_parent(struct clk_t * clk)
{
	struct clk_rk1808_pll_pdata_t * pdat = (struct clk_rk1808_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_rk1808_pll_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_rk1808_pll_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_rk1808_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_rk1808_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_rk1808_pll_pdata_t * pdat = (struct clk_rk1808_pll_pdata_t *)clk->priv;
	u64_t rate = 0;
	u32_t con, mode;
	u32_t postdiv1, postdiv2, fbdiv, refdiv;

	switch(pdat->channel)
	{
	case 0 ... 4:
		con = read32(pdat->cru + CRU_PLL_CON(pdat->channel, 0));
		postdiv1 = (con >> 12) & 0x7;
		fbdiv = (con >> 0) & 0xfff;
		con = read32(pdat->cru + CRU_PLL_CON(pdat->channel, 1));
		postdiv2 = (con >> 6) & 0x7;
		refdiv = (con >> 0) & 0x3f;
		mode = (read32(pdat->cru + CRU_MODE) >> (pdat->channel * 2)) & 0x3;
		break;

	case 5:
		return 0;

	default:
		return 0;
	}

	if(mode == 0x1)
	{
		rate = prate * fbdiv / (u64_t)(refdiv * postdiv1 * postdiv2);
	}
	else if(mode == 0x2)
	{
		rate = 32768;
	}
	else
	{
		rate = prate;
	}

	return rate;
}

static struct device_t * clk_rk1808_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rk1808_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 7)
		return NULL;

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rk1808_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->cru = phys_to_virt(RK1808_CRU_BASE);
	pdat->pmucru = phys_to_virt(RK1808_PMU_CRU_BASE);
	pdat->parent = strdup(parent);
	pdat->channel = channel;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rk1808_pll_set_parent;
	clk->get_parent = clk_rk1808_pll_get_parent;
	clk->set_enable = clk_rk1808_pll_set_enable;
	clk->get_enable = clk_rk1808_pll_get_enable;
	clk->set_rate = clk_rk1808_pll_set_rate;
	clk->get_rate = clk_rk1808_pll_get_rate;
	clk->priv = pdat;

	if(!(dev = register_clk(clk, drv)))
	{
		free(pdat->parent);
		free(clk->name);
		free(clk->priv);
		free(clk);
		return NULL;
	}
	if(dt_read_object(n, "default", &o))
	{
		char * c = clk->name;
		char * p;
		u64_t r;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_clk(p))
			clk_set_parent(c, p);
		if((r = (u64_t)dt_read_long(&o, "rate", 0)) > 0)
			clk_set_rate(c, r);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				clk_enable(c);
			else
				clk_disable(c);
		}
	}
	return dev;
}

static void clk_rk1808_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rk1808_pll_pdata_t * pdat = (struct clk_rk1808_pll_pdata_t *)clk->priv;

	if(clk)
	{
		unregister_clk(clk);
		free(pdat->parent);
		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rk1808_pll_suspend(struct device_t * dev)
{
}

static void clk_rk1808_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_rk1808_pll = {
	.name		= "clk-rk1808-pll",
	.probe		= clk_rk1808_pll_probe,
	.remove		= clk_rk1808_pll_remove,
	.suspend	= clk_rk1808_pll_suspend,
	.resume		= clk_rk1808_pll_resume,
};

static __init void clk_rk1808_pll_driver_init(void)
{
	register_driver(&clk_rk1808_pll);
}

static __exit void clk_rk1808_pll_driver_exit(void)
{
	unregister_driver(&clk_rk1808_pll);
}

driver_initcall(clk_rk1808_pll_driver_init);
driver_exitcall(clk_rk1808_pll_driver_exit);
