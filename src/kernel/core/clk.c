/*
 * kernel/core/clk.c
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
#include <xboot/clk.h>

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

	clk->count = 0;
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

static s32_t clk_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct clk_list_t * pos, * n;
	s8_t * p;
	s32_t len = 0;
	const char * name;
	const char * parent;
	u64_t rate;
	bool_t enable;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), "[clk]");

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		name = pos->clk->name;
		parent = clk_get_parent(name);
		rate = clk_get_rate(name);
		enable = clk_status(name);

		len += sprintf((char *)(p + len), "\r\n %s(%d): %Ld.%06LdMHZ %s %s", name, pos->clk->count, rate / (u64_t)(1000 * 1000), rate % (u64_t)(1000 * 1000), enable ? "Enable" : "Disable", parent ? parent : "None");
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
	free(p);

	return len;
}

static struct proc_t clk_proc = {
	.name	= "clk",
	.read	= clk_proc_read,
};

static __init void clk_proc_init(void)
{
	proc_register(&clk_proc);
}

static __exit void clk_proc_exit(void)
{
	proc_unregister(&clk_proc);
}

core_initcall(clk_proc_init);
core_exitcall(clk_proc_exit);
