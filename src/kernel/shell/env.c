/*
 * kernel/shell/env.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xml.h>
#include <hash.h>
#include <xboot/machine.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/env.h>
#include <fs/fsapi.h>

/*
 * the hash list of environment variable
 */
struct hlist_head env_hash[CONFIG_ENV_HASH_SIZE];

/*
 * default environment variable, must place NULL at the end.
 */
static struct env default_envcfg[] = {
	{
		.key	= "prompt",
		.value	= "xboot"
	}, {
		.key	= "linux-machtype",
		.value	= "0"
	}, {
		.key	= "linux-cmdline",
		.value	= "console=tty0"
	}, {
		.key	= NULL,
		.value	= NULL
	}
};

/*
 * find environment variable
 */
static struct env_list * env_find(const char * key)
{
	struct env_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!key)
		return NULL;

	hash = string_hash(key) % CONFIG_ENV_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(env_hash[hash]), node)
	{
		if(strcmp((x_s8*)list->env.key, (x_s8*)key) == 0)
			return list;
	}

	return NULL;
}

/*
 * add a environment variable. if it already existst, modify it.
 */
x_bool env_add(const char * key, const char * value)
{
	struct env_list * list;
	char * k, * v;
	x_u32 hash;

	if(!key)
		return FALSE;

	list = env_find(key);
	if(list)
	{
		if(strcmp((x_s8*)list->env.value, (x_s8*)value) == 0)
			return TRUE;
		else
		{
			free(list->env.value);
			list->env.value = malloc(strlen((x_s8*)value) + 1);
			if(!list->env.value)
			{
				list->env.value = NULL;
				return FALSE;
			}
			strcpy((x_s8*)list->env.value, (x_s8*)value);
			return TRUE;
		}
	}
	else
	{
		list = malloc(sizeof(struct env_list));
		if(!list)
			return FALSE;

		k = malloc(strlen((x_s8*)key) + 1);
		v = malloc(strlen((x_s8*)value) + 1);
		if(!k || !v)
		{
			free(list);
			free(k);
			free(v);
			return FALSE;
		}

		strcpy((x_s8*)k, (x_s8*)key);
		strcpy((x_s8*)v, (x_s8*)value);
		list->env.key = (char *)k;
		list->env.value = (char *)v;

		hash = string_hash(key) % CONFIG_ENV_HASH_SIZE;
		hlist_add_head(&(list->node), &(env_hash[hash]));

		return TRUE;
	}
}

/*
 * set a environment variable.
 */
x_bool env_set(const char * key, const char * value)
{
	struct env_list * list;

	list = env_find(key);
	if(list)
	{
		if(strcmp((x_s8*)list->env.value, (x_s8*)value) == 0)
			return TRUE;
		else
		{
			free(list->env.value);
			list->env.value = malloc(strlen((x_s8*)value) + 1);
			if(!list->env.value)
			{
				list->env.value = NULL;
				return FALSE;
			}
			strcpy((x_s8*)list->env.value, (x_s8*)value);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get a environment variable.
 */
char * env_get(const char * key)
{
	struct env_list * list;

	list = env_find(key);

	if(list)
		return list->env.value;

	return NULL;
}

/*
 * remove a environment variable.
 */
x_bool env_remove(const char * key)
{
	struct env_list * list;

	list = env_find(key);
	if(list)
	{
		free(list->env.key);
		free(list->env.value);
		hlist_del(&(list->node));
		free(list);
		return TRUE;
	}

	return FALSE;
}

/*
 * load environment variable from file
 */
x_bool env_load(char * file)
{
	struct machine * mach = get_machine();
	struct env_list * list;
	struct env * env;
	struct hlist_node * pos;
	struct xml * root, * child;
	x_s32 i;

	/* delete all env in hash list */
	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(env_hash[i]), node)
		{
			free(list->env.key);
			free(list->env.value);
			hlist_del(&(list->node));
			free(list);
		}
	}

	/* parse xml config file and load env */
	root = xml_parse_file(file);
	if(root)
	{
	    for(child = xml_child(root, "env"); child; child = child->next)
	    {
	    	env_add(xml_child(child, "key")->txt, xml_child(child, "value")->txt);
	    }

	    xml_free(root);
	}

	/* load machine's configure */
	if(mach && mach->cfg.env)
	{
		env = mach->cfg.env;
		while(env->key)
		{
			if(!env_get(env->key))
				env_add(env->key, env->value);
			env++;
		}
	}

	/* load system default configure */
	env = default_envcfg;
	while(env->key)
	{
		if(!env_get(env->key))
			env_add(env->key, env->value);
		env++;
	}

    return TRUE;
}

/*
 * save environment variable to file
 */
x_bool env_save(char * file)
{
	struct env_list * list;
	struct hlist_node * pos;
	struct xml * root, * env;
	struct xml * node;
	x_s32 fd;
	char * str;
	x_s32 i;

	root = xml_new("environment variable");

	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(env_hash[i]), node)
		{
			env = xml_add_child(root, "env", 0);

			node = xml_add_child(env, "key", 0);
			xml_set_txt(node, list->env.key);
			node = xml_add_child(env, "value", 1);
			xml_set_txt(node, list->env.value);
		}
	}

	str = xml_toxml(root);

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
		return FALSE;

	write(fd, str, strlen((const x_s8 *)str));
	close(fd);

	free(str);
	xml_free(root);

	return TRUE;
}

/*
 * env pure init
 */
static __init void env_pure_sync_init(void)
{
	x_s32 i;

	/* initialize env hash list */
	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
		init_hlist_head(&env_hash[i]);
}

module_init(env_pure_sync_init, LEVEL_PURE_SYNC);
