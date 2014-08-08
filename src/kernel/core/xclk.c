/*
 * kernel/core/xclk.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <spinlock.h>

struct xclk_t
{
	const char * name;
	int count;

	void (*set_parent)(struct xclk_t * clk, const char * name);
	char * (*get_parent)(struct xclk_t * clk);
	void (*set_enable)(struct xclk_t * clk, bool_t enable);
	bool_t (*get_enable)(struct xclk_t * clk);
	void (*set_rate)(struct xclk_t * clk, u64_t parent, u64_t rate);
	u64_t (*get_rate)(struct xclk_t * clk, u64_t parent);

	void * priv;
};

struct xclk_list_t
{
	struct xclk_t * clk;
	struct list_head entry;
};

static struct xclk_list_t __clk_list = {
	.entry = {
		.next	= &(__clk_list.entry),
		.prev	= &(__clk_list.entry),
	},
};
static spinlock_t __clk_list_lock = SPIN_LOCK_INIT();

static struct xclk_t * xclk_lookup(const char * name)
{
	struct xclk_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		if(strcmp(pos->clk->name, name) == 0)
			return pos->clk;
	}

	return NULL;
}

bool_t xclk_register(struct xclk_t * clk)
{
	struct xclk_list_t * cl;

	if(!clk || !clk->name)
		return FALSE;

	if(xclk_lookup(clk->name))
		return FALSE;

	cl = malloc(sizeof(struct xclk_list_t));
	if(!cl)
		return FALSE;

	clk->count = 0;
	cl->clk = clk;

	spin_lock_irq(&__clk_list_lock);
	list_add_tail(&cl->entry, &(__clk_list.entry));
	spin_unlock_irq(&__clk_list_lock);

	return TRUE;
}

bool_t xclk_unregister(struct xclk_t * clk)
{
	struct xclk_list_t * pos, * n;

	if(!clk || !clk->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		if(pos->clk == clk)
		{
			spin_lock_irq(&__clk_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__clk_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
/*
void xclk_set_parent(struct xclk_t * clk, const char * name)
{
	if(clk && clk->set_parent)
		clk->set_parent(clk, name);
}

struct xclk_t * xclk_get_parent(struct xclk_t * clk)
{
	if(!clk)
		return NULL;

	if(clk->get_parent)
		return xclk_lookup(clk->get_parent(clk));

	return NULL;
}
*/

void xclk_enable(const char * name)
{
	struct xclk_t * clk = xclk_lookup(name);

	if(!clk)
		return;

	if(clk->count == 0)
	{
		if(clk->get_parent)
			xclk_enable(clk->get_parent(clk));

		if(clk->set_enable)
			clk->set_enable(clk, TRUE);
	}
	clk->count++;
}

void xclk_disable(const char * name)
{
	struct xclk_t * clk = xclk_lookup(name);

	if(!clk)
		return;

	if(clk->count == 0)
		return;

	clk->count--;
	if(clk->count == 0)
	{
		if(clk->get_parent)
			xclk_disable(clk->get_parent(clk));

		if(clk->set_enable)
			clk->set_enable(clk, FALSE);
	}
}

bool_t xclk_status(const char * name)
{
	struct xclk_t * clk = xclk_lookup(name);

	if(!clk)
		return FALSE;

	if(clk->get_enable)
		return clk->get_enable(clk);
	return clk->count != 0 ? TRUE : FALSE;
}

void xclk_set_rate(const char * name, u64_t rate)
{
	struct xclk_t * clk = xclk_lookup(name);
	u64_t prate;

	if(!clk)
		return;

	prate = xclk_get_rate(clk->get_parent(clk));
	if(clk->set_rate)
		clk->set_rate(clk, prate, rate);
}

u64_t xclk_get_rate(const char * name)
{
	struct xclk_t * clk = xclk_lookup(name);
	u64_t prate;

	if(!clk)
		return 0;

	prate = xclk_get_rate(clk->get_parent(clk));
	if(clk->get_rate)
		return clk->get_rate(clk, prate);

	return 0;
}
