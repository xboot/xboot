/*
 * driver/clk-mbox.c
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
#include <bcm2836-mbox.h>

struct clk_mbox_t {
	char * name;
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

static bool_t register_clk_mbox(struct device_t ** device, struct clk_mbox_t * mclk)
{
	struct clk_t * clk;

	if(!mclk || !mclk->name)
		return FALSE;

	if(search_clk(mclk->name))
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

	if(!register_clk(device, clk))
	{
		free(clk);
		return FALSE;
	}
	return TRUE;
}

static bool_t unregister_clk_mbox(struct clk_mbox_t * mclk)
{
	struct clk_t * clk;

	if(!mclk || !mclk->name)
		return FALSE;

	clk = search_clk(mclk->name);
	if(!clk)
		return FALSE;

	if(unregister_clk(clk))
	{
		free(clk);
		return TRUE;
	}
	return FALSE;
}

static struct device_t * clk_mbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_mbox_t * mclk;
	struct device_t * dev;
	char * name = dt_read_string(n, "name", NULL);
	int id = dt_read_u64(n, "clock-id", -1);

	if(!name || id < 1 || id > 10)
		return NULL;

	mclk = malloc(sizeof(struct clk_mbox_t));
	if(!mclk)
		return NULL;

	mclk->name = strdup(name);
	mclk->id = id;

	if(!register_clk_mbox(&dev, mclk))
	{
		free(mclk->name);
		free(mclk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void clk_mbox_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_mbox_t * mclk = (struct clk_mbox_t *)clk->priv;

	if(mclk && unregister_clk_mbox(mclk))
	{
		free(mclk->name);
		free(mclk);
	}
}

static void clk_mbox_suspend(struct device_t * dev)
{
}

static void clk_mbox_resume(struct device_t * dev)
{
}

static struct driver_t clk_mbox = {
	.name		= "clk-mbox",
	.probe		= clk_mbox_probe,
	.remove		= clk_mbox_remove,
	.suspend	= clk_mbox_suspend,
	.resume		= clk_mbox_resume,
};

static __init void clk_mbox_driver_init(void)
{
	register_driver(&clk_mbox);
}

static __exit void clk_mbox_driver_exit(void)
{
	unregister_driver(&clk_mbox);
}

driver_initcall(clk_mbox_driver_init);
driver_exitcall(clk_mbox_driver_exit);
