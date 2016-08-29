/*
 * driver/clk/clk-link.c
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

static void clk_link_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_link_get_parent(struct clk_t * clk)
{
	struct clk_link_t * lclk = (struct clk_link_t *)clk->priv;
	return lclk->parent;
}

static void clk_link_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_link_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_link_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_link_get_rate(struct clk_t * clk, u64_t prate)
{
	return prate;
}

bool_t register_clk_link(struct device_t ** device, struct clk_link_t * lclk)
{
	struct clk_t * clk;

	if(!lclk || !lclk->name)
		return FALSE;

	if(search_clk(lclk->name))
		return FALSE;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
		return FALSE;

	clk->name = lclk->name;
	clk->type = CLK_TYPE_LINK;
	kref_init(&clk->count);
	clk->set_parent = clk_link_set_parent;
	clk->get_parent = clk_link_get_parent;
	clk->set_enable = clk_link_set_enable;
	clk->get_enable = clk_link_get_enable;
	clk->set_rate = clk_link_set_rate;
	clk->get_rate = clk_link_get_rate;
	clk->priv = lclk;

	if(!register_clk(device, clk))
	{
		free(clk);
		return FALSE;
	}
	return TRUE;
}

bool_t unregister_clk_link(struct clk_link_t * lclk)
{
	struct clk_t * clk;

	if(!lclk || !lclk->name)
		return FALSE;

	clk = search_clk(lclk->name);
	if(!clk)
		return FALSE;

	if(unregister_clk(clk))
	{
		free(clk);
		return TRUE;
	}
	return FALSE;
}

static struct device_t * clk_link_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_link_t * fclk;
	struct device_t * dev;
	char * name = dt_read_string(n, "name", NULL);
	char * parent = dt_read_string(n, "parent", NULL);

	if(!name || !parent)
		return NULL;

	fclk = malloc(sizeof(struct clk_fixed_t));
	if(!fclk)
		return NULL;

	fclk->name = strdup(name);
	fclk->parent = strdup(parent);

	if(!register_clk_link(&dev, fclk))
	{
		free(fclk->name);
		free(fclk->parent);
		free(fclk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void clk_link_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_link_t * fclk = (struct clk_link_t *)clk->priv;

	if(fclk && unregister_clk_link(fclk))
	{
		free(fclk->name);
		free(fclk->parent);
		free(fclk);
	}
}

static void clk_link_suspend(struct device_t * dev)
{
}

static void clk_link_resume(struct device_t * dev)
{
}

static struct driver_t clk_link = {
	.name		= "clk-link",
	.probe		= clk_link_probe,
	.remove		= clk_link_remove,
	.suspend	= clk_link_suspend,
	.resume		= clk_link_resume,
};

static __init void clk_link_driver_init(void)
{
	register_driver(&clk_link);
}

static __exit void clk_link_driver_exit(void)
{
	unregister_driver(&clk_link);
}

driver_initcall(clk_link_driver_init);
driver_exitcall(clk_link_driver_exit);
