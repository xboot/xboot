/*
 * kernel/core/clk.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

struct clk_list_t __clk_list = {
	.entry = {
		.next	= &(__clk_list.entry),
		.prev	= &(__clk_list.entry),
	},
};
static spinlock_t __clk_list_lock = SPIN_LOCK_INIT();

static struct clk_t * clk_search(const char * name)
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

bool_t clk_get_rate(const char * name, u64_t * rate)
{
	struct clk_t * clk;

	clk = clk_search(name);
	if(!clk)
		return FALSE;

	if(rate)
		*rate = clk->rate;
	return TRUE;
}

static s32_t clk_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct clk_list_t * pos, * n;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), "[clk]");

	list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
	{
		len += sprintf((char *)(p + len), "\r\n %s: %Ld.%06LdMHZ", pos->clk->name, pos->clk->rate / (u64_t)(1000 * 1000), pos->clk->rate % (u64_t)(1000 * 1000));
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
