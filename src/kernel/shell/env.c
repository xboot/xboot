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

#include <xboot.h>
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <charset.h>
#include <malloc.h>
#include <xml.h>
#include <hash.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/env.h>
#include <fs/fileio.h>

/*
 * the hash list of environment variable
 */
struct hlist_head env_hash[CONFIG_ENV_HASH_SIZE];

/*
 * find environment variable
 */
static struct env_list * env_find(const char * key)
{
	struct env_list * list;
	struct hlist_node * pos;
	u32_t hash;

	if(!key)
		return NULL;

	hash = string_hash(key) % CONFIG_ENV_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(env_hash[hash]), node)
	{
		if(strcmp(list->env.key, key) == 0)
			return list;
	}

	return NULL;
}

/*
 * get a environment variable.
 */
char * env_get(const char * key, const char * value)
{
	struct env_list * list;

	list = env_find(key);

	if(list && list->env.value)
		return list->env.value;

	return (char *)value;
}

/*
 * add a environment variable. if it already exist, modify it.
 */
bool_t env_add(const char * key, const char * value)
{
	struct env_list * list;
	u32_t hash;

	if(!key)
		return FALSE;

	list = env_find(key);
	if(list)
	{
		if(strcmp(list->env.value, value) == 0)
			return TRUE;
		else
		{
			if(list->env.value)
				free(list->env.value);
			list->env.value = strdup(value);
			return TRUE;
		}
	}
	else
	{
		list = malloc(sizeof(struct env_list));
		if(!list)
			return FALSE;

		list->env.key = strdup(key);
		list->env.value = strdup(value);

		hash = string_hash(key) % CONFIG_ENV_HASH_SIZE;
		hlist_add_head(&(list->node), &(env_hash[hash]));

		return TRUE;
	}
}

/*
 * remove a environment variable.
 */
bool_t env_remove(const char * key)
{
	struct env_list * list;

	list = env_find(key);
	if(list)
	{
		if(list->env.key)
			free(list->env.key);
		if(list->env.value)
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
bool_t env_load(char * file)
{
	struct env_list * list;
	struct hlist_node * pos, * n;
	struct xml * root, * env;
	struct xml * key, * value;
	s32_t i;

	/*
	 * check the xml file contained environment variable
	 */
	root = xml_parse_file(file);
	if(!root || !root->name)
		return FALSE;

	if(strcmp(root->name, "environment") != 0)
	{
		xml_free(root);
		return FALSE;
	}

	/*
	 * delete all of the environment variable
	 */
	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry_safe(list, pos, n, &(env_hash[i]), node)
		{
			hlist_del(&list->node);

			free(list->env.key);
			free(list->env.value);
			free(list);
		}
	}

	/*
	 * add environment variable
	 */
	for(env = xml_child(root, "env"); env; env = env->next)
	{
		key = xml_child(env, "key");
		value = xml_child(env, "value");

		if(key && value)
			env_add(key->txt, value->txt);
	}

	xml_free(root);
	return TRUE;
}

/*
 * save environment variable to file
 */
bool_t env_save(char * file)
{
	struct env_list * list;
	struct hlist_node * pos;
	struct xml * root, * env;
	struct xml * key, * value;
	s32_t fd;
	char * str;
	s32_t i;

	root = xml_new("environment");
	if(!root)
		return FALSE;

	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(env_hash[i]), node)
		{
			env = xml_add_child(root, "env", 0);

			key = xml_add_child(env, "key", 0);
			xml_set_txt(key, list->env.key);

			value = xml_add_child(env, "value", 1);
			xml_set_txt(value, list->env.value);
		}
	}

	str = xml_toxml(root);
	if(!str)
	{
		xml_free(root);
		return FALSE;
	}

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		free(str);
		xml_free(root);
		return FALSE;
	}

	write(fd, str, strlen(str));
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
	s32_t i;

	/* initialize env hash list */
	for(i = 0; i < CONFIG_ENV_HASH_SIZE; i++)
		init_hlist_head(&env_hash[i]);
}

module_init(env_pure_sync_init, LEVEL_PURE_SYNC);
