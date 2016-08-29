/*
 * driver/clk/clk.c
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

static const char * clk_type_to_string(const char * name)
{
	struct clk_t * clk = search_clk(name);
	char * type = "unkown";

	if(clk)
	{
		switch(clk->type)
		{
		case CLK_TYPE_FIXED:
			type = "fixed";
			break;
		case CLK_TYPE_FIXED_FACTOR:
			type = "fixed-factor";
			break;
		case CLK_TYPE_PLL:
			type = "pll";
			break;
		case CLK_TYPE_MUX:
			type = "mux";
			break;
		case CLK_TYPE_DIVIDER:
			type = "divider";
			break;
		case CLK_TYPE_GATE:
			type = "gate";
			break;
		case CLK_TYPE_LINK:
			type = "link";
			break;
		case CLK_TYPE_OTHER:
			type = "other";
			break;
		default:
			break;
		}
	}
	return type;
}

static ssize_t clk_read_dump(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	const char * name = clk->name;
	char * p = buf;
	int len = 0;
	u64_t rate;

	while(name)
	{
		rate = clk_get_rate(name);
		len += sprintf((char *)(p + len), " %s(%s): %Ld.%06LdMHZ %s\r\n", name, clk_type_to_string(name), rate / (u64_t)(1000 * 1000), rate % (u64_t)(1000 * 1000), clk_status(name) ? "enable" : "disable");
		name = clk_get_parent(name);
	}
	return len;
}

static ssize_t clk_read_type(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	return sprintf(buf, "%s", clk_type_to_string(clk->name));
}

static ssize_t clk_read_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	return sprintf(buf, "%d", clk_status(clk->name) ? 1 : 0);
}

static ssize_t clk_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	int enable = strtol(buf, NULL, 0);
	if(enable != 0)
		clk_enable(clk->name);
	else
		clk_disable(clk->name);
	return size;
}

static ssize_t clk_read_rate(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	u64_t rate = clk_get_rate(clk->name);
	return sprintf(buf, "%Ld.%06LdMHZ", rate / (u64_t)(1000 * 1000), rate % (u64_t)(1000 * 1000));
}

static ssize_t clk_write_rate(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	u64_t rate = strtoull(buf, NULL, 0);
	clk_set_rate(clk->name, rate);
	return size;
}

static ssize_t clk_read_parent(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	const char * parent = clk_get_parent(clk->name);
	return sprintf(buf, "%s", parent ? parent : "NONE");
}

static ssize_t clk_write_parent(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clk_t * clk = (struct clk_t *)kobj->priv;
	clk_set_parent(clk->name, buf);
	return size;
}

struct clk_t * search_clk(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_CLK);
	if(!dev)
		return NULL;

	return (struct clk_t *)dev->priv;
}

bool_t register_clk(struct device_t ** device, struct clk_t * clk)
{
	struct device_t * dev;

	if(!clk || !clk->name)
		return FALSE;

	if(search_clk(clk->name))
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(clk->name);
	dev->type = DEVICE_TYPE_CLK;
	dev->priv = clk;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "dump", clk_read_dump, NULL, clk);
	kobj_add_regular(dev->kobj, "type", clk_read_type, NULL, clk);
	kobj_add_regular(dev->kobj, "enable", clk_read_enable, clk_write_enable, clk);
	kobj_add_regular(dev->kobj, "rate", clk_read_rate, clk_write_rate, clk);
	kobj_add_regular(dev->kobj, "parent", clk_read_parent, clk_write_parent, clk);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_clk(struct clk_t * clk)
{
	struct device_t * dev;

	if(!clk || !clk->name)
		return FALSE;

	dev = search_device(clk->name, DEVICE_TYPE_CLK);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void clk_set_parent(const char * name, const char * pname)
{
	struct clk_t * clk = search_clk(name);
	struct clk_t * pclk = search_clk(pname);

	if(pclk && clk && clk->set_parent)
		clk->set_parent(clk, pname);
}

const char * clk_get_parent(const char * name)
{
	struct clk_t * clk = search_clk(name);

	if(clk && clk->get_parent)
		return clk->get_parent(clk);
	return NULL;
}

void clk_enable(const char * name)
{
	struct clk_t * clk = search_clk(name);

	if(!clk)
		return;

	if(clk->get_parent)
		clk_enable(clk->get_parent(clk));

	if(clk->set_enable)
		clk->set_enable(clk, TRUE);

	kref_get(&clk->count);
}

static void __clk_disable(struct kref_t * kref)
{
	struct clk_t * clk = container_of(kref, struct clk_t, count);

	if(clk->get_parent)
		clk_disable(clk->get_parent(clk));

	if(clk->set_enable)
		clk->set_enable(clk, FALSE);
}

void clk_disable(const char * name)
{
	struct clk_t * clk = search_clk(name);

	if(!clk)
		return;

	kref_put(&clk->count, __clk_disable);
}

bool_t clk_status(const char * name)
{
	struct clk_t * clk = search_clk(name);

	if(!clk)
		return FALSE;

	if(!clk->get_parent(clk))
		return clk->get_enable(clk);

	if(clk->get_enable(clk))
		return clk_status(clk->get_parent(clk));

	return FALSE;
}

void clk_set_rate(const char * name, u64_t rate)
{
	struct clk_t * clk = search_clk(name);
	u64_t prate;

	if(!clk)
		return;

	if(clk->get_parent)
		prate = clk_get_rate(clk->get_parent(clk));
	else
		prate = 0;

	if(clk->set_rate)
		clk->set_rate(clk, prate, rate);
}

u64_t clk_get_rate(const char * name)
{
	struct clk_t * clk = search_clk(name);
	u64_t prate;

	if(!clk)
		return 0;

	if(clk->get_parent)
		prate = clk_get_rate(clk->get_parent(clk));
	else
		prate = 0;

	if(clk->get_rate)
		return clk->get_rate(clk, prate);

	return 0;
}
