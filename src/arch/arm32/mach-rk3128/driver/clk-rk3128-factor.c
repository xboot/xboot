/*
 * driver/clk-rk3128-factor.c
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

#include <xboot.h>
#include <clk/clk.h>

struct clk_rk3128_factor_pdata_t {
	virtual_addr_t virt;
	char * parent;
};

static void clk_rk3128_factor_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_rk3128_factor_get_parent(struct clk_t * clk)
{
	struct clk_rk3128_factor_pdata_t * pdat = (struct clk_rk3128_factor_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_rk3128_factor_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_rk3128_factor_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static u64_t clk_rk3128_gcd(u64_t a, u64_t b)
{
	u64_t c;

	while(b != 0)
	{
		c = a % b;
		a = b;
		b = c;
	}
	return a;
}

static void clk_rk3128_factor_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_rk3128_factor_pdata_t * pdat = (struct clk_rk3128_factor_pdata_t *)clk->priv;
	u64_t cdiv;
	u32_t mult, div;

	if(rate == 0)
		rate = prate;

	cdiv = clk_rk3128_gcd(prate, rate);
	div = (prate / cdiv) & 0xffff;
	mult = (rate / cdiv) & 0xffff;
	write32(pdat->virt, (mult << 16) | (div << 0));
}

static u64_t clk_rk3128_factor_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_rk3128_factor_pdata_t * pdat = (struct clk_rk3128_factor_pdata_t *)clk->priv;
	u32_t val = read32(pdat->virt);

	return (prate * ((val >> 16) & 0xffff)) / ((val >> 0) & 0xffff);
}

static struct device_t * clk_rk3128_factor_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rk3128_factor_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rk3128_factor_pdata_t));
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

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rk3128_factor_set_parent;
	clk->get_parent = clk_rk3128_factor_get_parent;
	clk->set_enable = clk_rk3128_factor_set_enable;
	clk->get_enable = clk_rk3128_factor_get_enable;
	clk->set_rate = clk_rk3128_factor_set_rate;
	clk->get_rate = clk_rk3128_factor_get_rate;
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

static void clk_rk3128_factor_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rk3128_factor_pdata_t * pdat = (struct clk_rk3128_factor_pdata_t *)clk->priv;

	if(clk)
	{
		unregister_clk(clk);
		free(pdat->parent);
		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rk3128_factor_suspend(struct device_t * dev)
{
}

static void clk_rk3128_factor_resume(struct device_t * dev)
{
}

static struct driver_t clk_rk3128_factor = {
	.name		= "clk-rk3128-factor",
	.probe		= clk_rk3128_factor_probe,
	.remove		= clk_rk3128_factor_remove,
	.suspend	= clk_rk3128_factor_suspend,
	.resume		= clk_rk3128_factor_resume,
};

static __init void clk_rk3128_factor_driver_init(void)
{
	register_driver(&clk_rk3128_factor);
}

static __exit void clk_rk3128_factor_driver_exit(void)
{
	unregister_driver(&clk_rk3128_factor);
}

driver_initcall(clk_rk3128_factor_driver_init);
driver_exitcall(clk_rk3128_factor_driver_exit);
