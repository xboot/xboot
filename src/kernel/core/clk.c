/*
 * xboot/kernel/core/clk.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <macros.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <div64.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/clk.h>

/*
 * the hash list of clk
 */
static struct hlist_head clk_hash[CONFIG_CLK_HASH_SIZE] = {{0}};

/*
 * search clk by name. a static function.
 */
static struct clk * clk_search(const char *name)
{
	struct clk_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return NULL;

	hash = string_hash(name) % CONFIG_CLK_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(clk_hash[hash]), node)
	{
		if(strcmp((x_s8*)list->clk->name, (const x_s8 *)name) == 0)
			return list->clk;
	}

	return NULL;
}

/*
 * register a clk into clk_list
 * return true is successed, otherwise is not.
 */
x_bool clk_register(struct clk * clk)
{
	struct clk_list * list;
	x_u32 hash;

	list = malloc(sizeof(struct clk_list));
	if(!list || !clk)
	{
		free(list);
		return FALSE;
	}

	if(!clk->name || clk_search(clk->name))
	{
		free(list);
		return FALSE;
	}

	list->clk = clk;

	hash = string_hash(clk->name) % CONFIG_CLK_HASH_SIZE;
	hlist_add_head(&(list->node), &(clk_hash[hash]));

	return TRUE;
}

/*
 * unregister clk from clk_list
 */
x_bool clk_unregister(struct clk * clk)
{
	struct clk_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!clk || !clk->name)
		return FALSE;

	hash = string_hash(clk->name) % CONFIG_CLK_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(clk_hash[hash]), node)
	{
		if(list->clk == clk)
		{
			hlist_del(&(list->node));
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get clk's rate.
 */
x_bool clk_get_rate(const char *name, x_u64 * rate)
{
	struct clk * clk;

	clk = clk_search(name);

	if(!clk)
		return FALSE;

	if(rate)
		*rate = clk->rate;

	return TRUE;
}

/*
 * clk proc interface
 */
static x_s32 clk_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct clk_list * list;
	struct hlist_node * pos;
	x_s8 * p;
	x_s32 i, len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[clk]");
	for(i = 0; i < CONFIG_CLK_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(clk_hash[i]), node)
		{
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s: %Ld.%06LdMHZ", list->clk->name, (x_u64)div64(list->clk->rate, 1000*1000), (x_u64)mod64(list->clk->rate, 1000*1000));
		}
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc clk_proc = {
	.name	= "clk",
	.read	= clk_proc_read,
};

/*
 * clk pure sync init
 */
static __init void clk_pure_sync_init(void)
{
	x_s32 i;

	/* initialize clk hash list */
	for(i = 0; i < CONFIG_CLK_HASH_SIZE; i++)
		init_hlist_head(&clk_hash[i]);

	/* register clk proc interface */
	proc_register(&clk_proc);
}

static __exit void clk_pure_sync_exit(void)
{
	/* unregister clk proc interface */
	proc_unregister(&clk_proc);
}

module_init(clk_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(clk_pure_sync_exit, LEVEL_PURE_SYNC);
