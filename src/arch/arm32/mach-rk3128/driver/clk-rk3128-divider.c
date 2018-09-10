/*
 * driver/clk-rk3128-divider.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <clk/clk.h>

struct clk_rk3128_divider_pdata_t {
	virtual_addr_t virt;
	char * parent;
	int shift;
	int width;
	int onebased;
};

static void clk_rk3128_divider_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_rk3128_divider_get_parent(struct clk_t * clk)
{
	struct clk_rk3128_divider_pdata_t * pdat = (struct clk_rk3128_divider_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_rk3128_divider_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_rk3128_divider_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_rk3128_divider_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_rk3128_divider_pdata_t * pdat = (struct clk_rk3128_divider_pdata_t *)clk->priv;
	u32_t mask = ((1 << (pdat->width)) - 1);
	u32_t div;
	u32_t val;

	if(rate == 0)
		rate = prate;

	div = prate / rate;
	if(pdat->onebased)
		div--;
	if(div > mask)
		div = mask;

	val = div << pdat->shift;
	val |= mask << (pdat->shift + 16);
	write32(pdat->virt, val);
}

static u64_t clk_rk3128_divider_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_rk3128_divider_pdata_t * pdat = (struct clk_rk3128_divider_pdata_t *)clk->priv;
	u32_t mask = ((1 << (pdat->width)) - 1);
	u32_t div;

	div = read32(pdat->virt) >> pdat->shift;
	div &= mask;

	if(pdat->onebased)
		div++;
	if(div == 0)
		div = 1;
	return prate / div;
}

static struct device_t * clk_rk3128_divider_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rk3128_divider_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int shift = dt_read_int(n, "shift", -1);
	int width = dt_read_int(n, "width", -1);

	if(!parent || !name || (shift < 0) || (width <= 0))
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rk3128_divider_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->parent = strdup(parent);
	pdat->shift = shift;
	pdat->width = width;
	pdat->onebased = dt_read_bool(n, "divider-one-based", 1);

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rk3128_divider_set_parent;
	clk->get_parent = clk_rk3128_divider_get_parent;
	clk->set_enable = clk_rk3128_divider_set_enable;
	clk->get_enable = clk_rk3128_divider_get_enable;
	clk->set_rate = clk_rk3128_divider_set_rate;
	clk->get_rate = clk_rk3128_divider_get_rate;
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

static void clk_rk3128_divider_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rk3128_divider_pdata_t * pdat = (struct clk_rk3128_divider_pdata_t *)clk->priv;

	if(clk && unregister_clk(clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rk3128_divider_suspend(struct device_t * dev)
{
}

static void clk_rk3128_divider_resume(struct device_t * dev)
{
}

static struct driver_t clk_rk3128_divider = {
	.name		= "clk-rk3128-divider",
	.probe		= clk_rk3128_divider_probe,
	.remove		= clk_rk3128_divider_remove,
	.suspend	= clk_rk3128_divider_suspend,
	.resume		= clk_rk3128_divider_resume,
};

static __init void clk_rk3128_divider_driver_init(void)
{
	register_driver(&clk_rk3128_divider);
}

static __exit void clk_rk3128_divider_driver_exit(void)
{
	unregister_driver(&clk_rk3128_divider);
}

driver_initcall(clk_rk3128_divider_driver_init);
driver_exitcall(clk_rk3128_divider_driver_exit);
