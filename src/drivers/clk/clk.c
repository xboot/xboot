/*
 * driver/clk/clk.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

struct clk_list_t
{
	struct clk_t * clk;
	struct list_head entry;
};

static struct clk_list_t __clk_list = {
	.entry = {
		.next	= &(__clk_list.entry),
		.prev	= &(__clk_list.entry),
	},
};
static spinlock_t __clk_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_clk_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clk");
}

static const char * clk_type_to_string(const char * name)
{
	struct clk_t * clk = clk_search(name);

	if(!clk)
		return "unkown";

	switch(clk->type)
	{
	case CLK_TYPE_FIXED:
		return "fixed";
	case CLK_TYPE_FIXED_FACTOR:
		return "fixed-factor";
	case CLK_TYPE_PLL:
		return "pll";
	case CLK_TYPE_MUX:
		return "mux";
	case CLK_TYPE_DIVIDER:
		return "divider";
	case CLK_TYPE_GATE:
		return "gate";
	default:
		break;
	}
	return "unkown";
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

struct clk_t * clk_search(const char * name)
{
	struct clk_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		if(strcmp(pos->clk->name, name) == 0)
			return pos->clk;
	}

	return NULL;
}

bool_t clk_register(struct clk_t * clk)
{
	struct clk_list_t * cl;

	if(!clk || !clk->name)
		return FALSE;

	if(clk_search(clk->name))
		return FALSE;

	cl = malloc(sizeof(struct clk_list_t));
	if(!cl)
		return FALSE;

	clk->kobj = kobj_alloc_directory(clk->name);
	kobj_add_regular(clk->kobj, "dump", clk_read_dump, NULL, clk);
	kobj_add_regular(clk->kobj, "type", clk_read_type, NULL, clk);
	kobj_add_regular(clk->kobj, "enable", clk_read_enable, clk_write_enable, clk);
	kobj_add_regular(clk->kobj, "rate", clk_read_rate, clk_write_rate, clk);
	kobj_add_regular(clk->kobj, "parent", clk_read_parent, clk_write_parent, clk);
	kobj_add(search_class_clk_kobj(), clk->kobj);
	cl->clk = clk;

	spin_lock_irq(&__clk_list_lock);
	list_add_tail(&cl->entry, &(__clk_list.entry));
	spin_unlock_irq(&__clk_list_lock);

	return TRUE;
}

bool_t clk_unregister(struct clk_t * clk)
{
	struct clk_list_t * pos, * n;

	if(!clk || !clk->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		if(pos->clk == clk)
		{
			spin_lock_irq(&__clk_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__clk_list_lock);

			kobj_remove(search_class_clk_kobj(), pos->clk->kobj);
			kobj_remove_self(clk->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void clk_set_parent(const char * name, const char * pname)
{
	struct clk_t * clk = clk_search(name);
	struct clk_t * pclk = clk_search(pname);

	if(pclk && clk && clk->set_parent)
		clk->set_parent(clk, pname);
}

const char * clk_get_parent(const char * name)
{
	struct clk_t * clk = clk_search(name);

	if(clk && clk->get_parent)
		return clk->get_parent(clk);
	return NULL;
}

void clk_enable(const char * name)
{
	struct clk_t * clk = clk_search(name);

	if(!clk)
		return;

	if(clk->get_parent)
		clk_enable(clk->get_parent(clk));

	if(clk->set_enable)
		clk->set_enable(clk, TRUE);

	clk->count++;
}

void clk_disable(const char * name)
{
	struct clk_t * clk = clk_search(name);

	if(!clk)
		return;

	if(clk->count > 0)
		clk->count--;

	if(clk->count == 0)
	{
		if(clk->get_parent)
			clk_disable(clk->get_parent(clk));

		if(clk->set_enable)
			clk->set_enable(clk, FALSE);
	}
}

bool_t clk_status(const char * name)
{
	struct clk_t * clk = clk_search(name);

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
	struct clk_t * clk = clk_search(name);
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
	struct clk_t * clk = clk_search(name);
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
