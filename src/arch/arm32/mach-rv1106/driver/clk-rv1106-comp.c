/*
 * driver/clk/clk-rv1106-comp.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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

struct clk_rv1106_comp_parent_t {
	char * name;
	int value;
};

struct clk_rv1106_comp_pdata_t {
	char * parent_nomux;
	struct clk_rv1106_comp_parent_t * parent;
	int nparent;
	virtual_addr_t mux_offset;
	int mux_shift;
	int mux_width;
	virtual_addr_t divider_offset;
	int divider_shift;
	int divider_width;
	int divider_onebased;
	virtual_addr_t gate_offset;
	int gate_shift;
	int gate_invert;
};

static void clk_rv1106_comp_set_parent(struct clk_t * clk, const char * pname)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->parent)
	{
		for(int i = 0; i < pdat->nparent; i++)
		{
			if(strcmp(pdat->parent[i].name, pname) == 0)
			{
				u32_t val = pdat->parent[i].value << pdat->mux_shift;
				val |= ((1 << pdat->mux_width) - 1) << (pdat->mux_shift + 16);
				write32(pdat->mux_offset, val);
				break;
			}
		}
	}
}

static const char * clk_rv1106_comp_get_parent(struct clk_t * clk)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->parent)
	{
		int val = (read32(pdat->mux_offset) >> pdat->mux_shift) & ((1 << pdat->mux_width) - 1);
		for(int i = 0; i < pdat->nparent; i++)
		{
			if(pdat->parent[i].value == val)
				return pdat->parent[i].name;
		}
	}
	else if(pdat->parent_nomux)
		return pdat->parent_nomux;
	return NULL;
}

static void clk_rv1106_comp_set_enable(struct clk_t * clk, bool_t enable)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->gate_offset != 0)
	{
		u32_t val;
		if(enable)
			val = (pdat->gate_invert ? 0x0 : 0x1) << pdat->gate_shift;
		else
			val = (pdat->gate_invert ? 0x1 : 0x0) << pdat->gate_shift;
		val |= 0x1 << (pdat->gate_shift + 16);
		write32(pdat->gate_offset, val);
	}
}

static bool_t clk_rv1106_comp_get_enable(struct clk_t * clk)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->gate_offset != 0)
	{
		if(read32(pdat->gate_offset) & (0x1 << pdat->gate_shift))
			return pdat->gate_invert ? FALSE : TRUE;
		return pdat->gate_invert ? TRUE : FALSE;
	}
	return TRUE;
}

static void clk_rv1106_comp_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->divider_offset != 0)
	{
		u32_t mask = ((1 << (pdat->divider_width)) - 1);
		u32_t div, val;
		if(rate == 0)
			rate = prate;
		div = prate / rate;
		if(pdat->divider_onebased)
			div--;
		if(div > mask)
			div = mask;
		val = div << pdat->divider_shift;
		val |= mask << (pdat->divider_shift + 16);
		write32(pdat->divider_offset, val);
	}
}

static u64_t clk_rv1106_comp_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;

	if(pdat->divider_offset != 0)
	{
		u32_t mask = ((1 << (pdat->divider_width)) - 1);
		u32_t div;
		div = read32(pdat->divider_offset) >> pdat->divider_shift;
		div &= mask;
		if(pdat->divider_onebased)
			div++;
		if(div == 0)
			div = 1;
		return prate / div;
	}
	return prate;
}


static struct device_t * clk_rv1106_comp_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rv1106_comp_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	char * name = dt_read_string(n, "name", NULL);
	int nparent;
	int i;

	if(!name)
		return NULL;

	if(search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rv1106_comp_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	if((nparent = dt_read_array_length(n, "parent")) > 0)
	{
		struct clk_rv1106_comp_parent_t * parent = malloc(sizeof(struct clk_rv1106_comp_parent_t) * nparent);
		if(parent)
		{
			for(i = 0; i < nparent; i++)
			{
				dt_read_array_object(n, "parent", i, &o);
				parent[i].name = strdup(dt_read_string(&o, "name", NULL));
				parent[i].value = dt_read_int(&o, "value", 0);
			}
		}
		pdat->parent_nomux = NULL;
		pdat->parent = parent;
		pdat->nparent = nparent;
	}
	else
	{
		char * p =  dt_read_string(n, "parent", NULL);
		if(!p || !search_clk(p))
		{
			free(pdat);
			free(clk);
			return NULL;
		}
		pdat->parent_nomux = strdup(p);
		pdat->parent = NULL;
		pdat->nparent = 0;
	}
	pdat->mux_offset = phys_to_virt((physical_addr_t)strtoull(dt_read_string(n, "mux-offset", "0"), NULL, 0));
	pdat->mux_shift = dt_read_int(n, "mux-shift", 0);
	pdat->mux_width = dt_read_int(n, "mux-width", 0);
	pdat->divider_offset = phys_to_virt((physical_addr_t)strtoull(dt_read_string(n, "divider-offset", "0"), NULL, 0));
	pdat->divider_shift = dt_read_int(n, "divider-shift", 0);
	pdat->divider_width = dt_read_int(n, "divider-width", 0);
	pdat->divider_onebased = dt_read_bool(n, "divider-one-based", 1);
	pdat->gate_offset = phys_to_virt((physical_addr_t)strtoull(dt_read_string(n, "gate-offset", "0"), NULL, 0));
	pdat->gate_shift = dt_read_int(n, "gate-shift", 0);
	pdat->gate_invert = dt_read_bool(n, "gate-invert", 0);

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rv1106_comp_set_parent;
	clk->get_parent = clk_rv1106_comp_get_parent;
	clk->set_enable = clk_rv1106_comp_set_enable;
	clk->get_enable = clk_rv1106_comp_get_enable;
	clk->set_rate = clk_rv1106_comp_set_rate;
	clk->get_rate = clk_rv1106_comp_get_rate;
	clk->priv = pdat;

	if(!(dev = register_clk(clk, drv)))
	{
		if(pdat->parent)
		{
			for(i = 0; i < pdat->nparent; i++)
				free(pdat->parent[i].name);
			free(pdat->parent);
		}
		if(pdat->parent_nomux)
			free(pdat->parent_nomux);
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

static void clk_rv1106_comp_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rv1106_comp_pdata_t * pdat = (struct clk_rv1106_comp_pdata_t *)clk->priv;
	int i;

	if(clk)
	{
		unregister_clk(clk);
		if(pdat->parent)
		{
			for(i = 0; i < pdat->nparent; i++)
				free(pdat->parent[i].name);
			free(pdat->parent);
		}
		if(pdat->parent_nomux)
			free(pdat->parent_nomux);
		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rv1106_comp_suspend(struct device_t * dev)
{
}

static void clk_rv1106_comp_resume(struct device_t * dev)
{
}

static struct driver_t clk_rv1106_comp = {
	.name		= "clk-rv1106-comp",
	.probe		= clk_rv1106_comp_probe,
	.remove		= clk_rv1106_comp_remove,
	.suspend	= clk_rv1106_comp_suspend,
	.resume		= clk_rv1106_comp_resume,
};

static __init void clk_rv1106_comp_driver_init(void)
{
	register_driver(&clk_rv1106_comp);
}

static __exit void clk_rv1106_comp_driver_exit(void)
{
	unregister_driver(&clk_rv1106_comp);
}

driver_initcall(clk_rv1106_comp_driver_init);
driver_exitcall(clk_rv1106_comp_driver_exit);
