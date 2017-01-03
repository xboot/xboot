/*
 * driver/clk-rk3399-mux.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>

struct clk_rk3399_mux_parent_t {
	char * name;
	int value;
};

struct clk_rk3399_mux_pdata_t {
	virtual_addr_t virt;
	struct clk_rk3399_mux_parent_t * parent;
	int nparent;
	int shift;
	int width;
};

static void clk_rk3399_mux_set_parent(struct clk_t * clk, const char * pname)
{
	struct clk_rk3399_mux_pdata_t * pdat = (struct clk_rk3399_mux_pdata_t *)clk->priv;
	u32_t val;
	int i;

	for(i = 0; i < pdat->nparent; i++)
	{
		if(strcmp(pdat->parent[i].name, pname) == 0)
		{
			val = read32(pdat->virt);
			val &= ~(((1 << pdat->width) - 1) << pdat->shift);
			val |= pdat->parent[i].value << pdat->shift;
			val |= ((1 << pdat->width) - 1) << (pdat->shift + 16);
			write32(pdat->virt, val);
			break;
		}
	}
}

static const char * clk_rk3399_mux_get_parent(struct clk_t * clk)
{
	struct clk_rk3399_mux_pdata_t * pdat = (struct clk_rk3399_mux_pdata_t *)clk->priv;
	int val = (read32(pdat->virt) >> pdat->shift) & ((1 << pdat->width) - 1);
	int i;

	for(i = 0; i < pdat->nparent; i++)
	{
		if(pdat->parent[i].value == val)
			return pdat->parent[i].name;
	}
	return NULL;
}

static void clk_rk3399_mux_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_rk3399_mux_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_rk3399_mux_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_rk3399_mux_get_rate(struct clk_t * clk, u64_t prate)
{
	return prate;
}


static struct device_t * clk_rk3399_mux_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_rk3399_mux_pdata_t * pdat;
	struct clk_rk3399_mux_parent_t * parent;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * name = dt_read_string(n, "name", NULL);
	int nparent = dt_read_array_length(n, "parent");
	int shift = dt_read_int(n, "shift", -1);
	int width = dt_read_int(n, "width", -1);
	int i;

	if(!name || (nparent <= 0) || (shift < 0) || (width <= 0))
		return NULL;

	if(search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_rk3399_mux_pdata_t));
	if(!pdat)
		return NULL;

	parent = malloc(sizeof(struct clk_rk3399_mux_parent_t) * nparent);
	if(!parent)
	{
		free(pdat);
		return NULL;
	}

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		free(parent);
		return NULL;
	}

	for(i = 0; i < nparent; i++)
	{
		dt_read_array_object(n, "parent", i, &o);
		parent[i].name = strdup(dt_read_string(&o, "name", NULL));
		parent[i].value = dt_read_int(&o, "value", 0);
	}

	pdat->virt = virt;
	pdat->parent = parent;
	pdat->nparent = nparent;
	pdat->shift = shift;
	pdat->width = width;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_rk3399_mux_set_parent;
	clk->get_parent = clk_rk3399_mux_get_parent;
	clk->set_enable = clk_rk3399_mux_set_enable;
	clk->get_enable = clk_rk3399_mux_get_enable;
	clk->set_rate = clk_rk3399_mux_set_rate;
	clk->get_rate = clk_rk3399_mux_get_rate;
	clk->priv = pdat;

	if(!register_clk(&dev, clk))
	{
		for(i = 0; i < pdat->nparent; i++)
			free(pdat->parent[i].name);
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

static void clk_rk3399_mux_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_rk3399_mux_pdata_t * pdat = (struct clk_rk3399_mux_pdata_t *)clk->priv;
	int i;

	if(clk && unregister_clk(clk))
	{
		for(i = 0; i < pdat->nparent; i++)
			free(pdat->parent[i].name);
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_rk3399_mux_suspend(struct device_t * dev)
{
}

static void clk_rk3399_mux_resume(struct device_t * dev)
{
}

static struct driver_t clk_rk3399_mux = {
	.name		= "clk-rk3399-mux",
	.probe		= clk_rk3399_mux_probe,
	.remove		= clk_rk3399_mux_remove,
	.suspend	= clk_rk3399_mux_suspend,
	.resume		= clk_rk3399_mux_resume,
};

static __init void clk_rk3399_mux_driver_init(void)
{
	register_driver(&clk_rk3399_mux);
}

static __exit void clk_rk3399_mux_driver_exit(void)
{
	unregister_driver(&clk_rk3399_mux);
}

driver_initcall(clk_rk3399_mux_driver_init);
driver_exitcall(clk_rk3399_mux_driver_exit);
