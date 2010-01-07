/*
 * drivers/terminal/terminal.c
 *
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
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/major.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <serial/serial.h>
#include <terminal/terminal.h>

/*
 * the hash list of stdout and stdin
 */
struct hlist_head stdout_list;
struct hlist_head stdin_list;

/*
 * terminal stdout number.
 */
static x_u32 terminal_stdout_num = 0;

/*
 * the hash list of terminal.
 */
static struct hlist_head terminal_hash[CONFIG_TERMINAL_HASH_SIZE];

/*
 * search terminal by name
 */
struct terminal * search_terminal(const char *name)
{
	struct terminal_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return NULL;

	hash = string_hash(name) % CONFIG_TERMINAL_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(terminal_hash[hash]), node)
	{
		if(strcmp((x_s8*)list->term->name, (const x_s8 *)name) == 0)
			return list->term;
	}

	return NULL;
}

/*
 * register a terminal into termial_list
 * return true is successed, otherwise is not.
 */
x_bool register_terminal(struct terminal * term)
{
	struct terminal_list * list;
	x_u32 hash;

	list = malloc(sizeof(struct terminal_list));
	if(!list || !term)
	{
		free(list);
		return FALSE;
	}

	if(!term->name || search_terminal(term->name))
	{
		free(list);
		return FALSE;
	}

	list->term = term;

	hash = string_hash(term->name) % CONFIG_TERMINAL_HASH_SIZE;
	hlist_add_head(&(list->node), &(terminal_hash[hash]));

	return TRUE;
}

/*
 * unregister terminal from terminal_list
 */
x_bool unregister_terminal(struct terminal * term)
{
	struct terminal_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!term || !term->name)
		return FALSE;

	hash = string_hash(term->name) % CONFIG_TERMINAL_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(terminal_hash[hash]), node)
	{
		if(list->term == term)
		{
			del_terminal_stdout(term->name);
			del_terminal_stdin(term->name);

			hlist_del(&(list->node));
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * search stdout from terminal stdout list by name.
 */
struct terminal_stdout_list * search_terminal_stdout(const char * name)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;

	if(!name)
		return NULL;

	hlist_for_each_entry(list,  pos, &stdout_list, node)
	{
		if(strcmp((x_s8*)list->term->name, (const x_s8 *)name) == 0)
			return list;
	}

	return NULL;
}

/*
 * add stdout terminal.
 */
x_bool add_terminal_stdout(const char * name)
{
	struct terminal_stdout_list * list;
	struct terminal * term;

	list = malloc(sizeof(struct terminal_stdout_list));
	if(!list || !name)
	{
		free(list);
		return FALSE;
	}

	if(search_terminal_stdout(name))
	{
		free(list);
		return FALSE;
	}

	term = search_terminal(name);
	if(term && term->write)
	{
		list->term = term;
		list->write = term->write;
		hlist_add_head(&(list->node), &stdout_list);
		terminal_stdout_num++;
		return TRUE;
	}
	else
	{
		free(list);
		return FALSE;
	}
}

/*
 * delete stdout terminal.
 */
x_bool del_terminal_stdout(const char * name)
{
	struct terminal_stdout_list * list;

	list = search_terminal_stdout(name);
	if(list)
	{
		hlist_del(&(list->node));
		free(list);
		terminal_stdout_num--;
		return TRUE;
	}

	return FALSE;
}

/*
 * search stdin from terminal stdin list by name.
 */
struct terminal_stdin_list * search_terminal_stdin(const char * name)
{
	struct terminal_stdin_list * list;
	struct hlist_node * pos;

	if(!name)
		return NULL;

	hlist_for_each_entry(list,  pos, &stdin_list, node)
	{
		if(strcmp((x_s8*)list->term->name, (const x_s8 *)name) == 0)
			return list;
	}

	return NULL;
}

/*
 * add stdin terminal.
 */
x_bool add_terminal_stdin(const char * name)
{
	struct terminal_stdin_list * list;
	struct terminal * term;

	list = malloc(sizeof(struct terminal_stdin_list));
	if(!list || !name)
	{
		free(list);
		return FALSE;
	}

	if(search_terminal_stdin(name))
	{
		free(list);
		return FALSE;
	}

	term = search_terminal(name);
	if(term && term->read)
	{
		list->term = term;
		list->read = term->read;
		hlist_add_head(&(list->node), &stdin_list);
		return TRUE;
	}
	else
	{
		free(list);
		return FALSE;
	}
}

/*
 * delete stdin terminal.
 */
x_bool del_terminal_stdin(const char * name)
{
	struct terminal_stdin_list * list;

	list = search_terminal_stdin(name);
	if(list)
	{
		hlist_del(&(list->node));
		free(list);
		return TRUE;
	}

	return FALSE;
}

/*
 * have stdout terminal, return true if have stdout.
 */
x_bool have_stdout_terminal(void)
{
	if(terminal_stdout_num > 0)
		return TRUE;
	return FALSE;
}

/*
 * get suitable width and height from stdout terminal list.
 */
x_bool stdout_terminal_getwh(x_s32 * w, x_s32 * h)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s32 width, height;

	if(!w || !h)
		return FALSE;

	if(have_stdout_terminal())
	{
		list = hlist_entry((&stdout_list)->first, struct terminal_stdout_list, node);
		list->term->getwh(list->term, w, h);

		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			list->term->getwh(list->term, &width, &height);
			if(width < *w)
				*w = width;
			if(height < *h)
				*h = height;
		}
		return TRUE;
	}

	return FALSE;
}

/*
 * terminal proc interface
 */
static x_s32 terminal_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct terminal_stdin_list * stdin;
	struct terminal_stdout_list * stdout;
	struct hlist_node * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[stdin]");
	hlist_for_each_entry(stdin,  pos, &stdin_list, node)
	{
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", stdin->term->name);
	}

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n[stdout]");
	hlist_for_each_entry(stdout,  pos, &stdout_list, node)
	{
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", stdout->term->name);
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

static struct proc terminal_proc = {
	.name	= "terminal",
	.read	= terminal_proc_read,
};

/*
 * terminal pure sync init
 */
static __init void terminal_pure_sync_init(void)
{
	x_s32 i;

	/* initialize terminal hash list */
	for(i = 0; i < CONFIG_TERMINAL_HASH_SIZE; i++)
		init_hlist_head(&terminal_hash[i]);

	/* initialize stdout and stdin */
	init_hlist_head(&stdout_list);
	init_hlist_head(&stdin_list);

	/* initialize the number of stdout */
	terminal_stdout_num = 0;

	/* register terminal proc interface */
	proc_register(&terminal_proc);
}

static __exit void terminal_pure_sync_exit(void)
{
	/* unregister terminal proc interface */
	proc_unregister(&terminal_proc);
}

module_init(terminal_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(terminal_pure_sync_exit, LEVEL_PURE_SYNC);
