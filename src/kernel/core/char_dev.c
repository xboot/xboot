/*
 * kernel/core/char_dev.c
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
#include <xboot.h>
#include <malloc.h>
#include <xboot/chrdev.h>


/*
 * the char device's hash table.
 */
struct hlist_head chrdev_hash[MAJOR_CHAR_MAX];

/*
 * char device hash function.
 */
inline x_u32 chrdev_major_to_index(x_u32 major)
{
	return (major % MAJOR_CHAR_MAX);
}

/*
 * search chrdev by major and name.
 */
struct chrdev * search_chrdev_by_major_name(x_u32 major, const char *name)
{
	struct chrdev_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return NULL;

	hash = chrdev_major_to_index(major);

	hlist_for_each_entry(list,  pos, &(chrdev_hash[hash]), node)
	{
		if(strcmp((x_s8 *)list->dev->name, (const x_s8 *)name) == 0)
			return list->dev;
	}

	return NULL;
}

/*
 * search chrdev by major and minor.
 */
struct chrdev * search_chrdev_by_major_minor(x_u32 major, x_u32 minor)
{
	struct chrdev_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	hash = chrdev_major_to_index(major);

	hlist_for_each_entry(list,  pos, &(chrdev_hash[hash]), node)
	{
		if(list->dev->minor == minor)
			return list->dev;
	}

	return NULL;
}

/*
 * register a major number for character devices.
 * this function registers a sort minor numbers.
 * the first minor number is 0.
 */
x_bool register_chrdev(x_u32 major, const char *name, const struct char_operations *ops)
{
	struct chrdev_list * list;
	struct chrdev * dev;
	x_u32 hash;
	x_u32 minor = 0;

	if(!ops)
		return FALSE;

	if(!name || search_chrdev_by_major_name(major, name))
		return FALSE;

	list = malloc(sizeof(struct chrdev_list));
	dev = malloc(sizeof(struct chrdev));
	if(!list || !dev)
	{
		free(list);
		free(dev);
		return FALSE;
	}

	for(;;)
	{
		if(!search_chrdev_by_major_minor(major, minor))
			break;
		minor++;
	}

	dev->name[32] = 0;
	strncpy((x_s8 *)dev->name, (const x_s8 *)name, 32);
	dev->major = major;
	dev->minor = minor;
	dev->ops = (struct char_operations *)ops;
	list->dev = dev;

	hash = chrdev_major_to_index(major);
	hlist_add_head(&(list->node), &(chrdev_hash[hash]));

	return TRUE;
}

/*
 * unregister char device. by major and name
 */
x_bool unregister_chrdev(x_u32 major, const char *name)
{
	struct chrdev_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return FALSE;

	hash = chrdev_major_to_index(major);

	hlist_for_each_entry(list,  pos, &(chrdev_hash[hash]), node)
	{
		if(strcmp((x_s8 *)list->dev->name, (const x_s8 *)name) == 0)
		{
			hlist_del(&(list->node));
			free(list->dev);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * char device pure sync init
 */
static __init void chrdev_pure_sync_init(void)
{
	x_s32 i;

	/* initialize platform hash list */
	for(i = 0; i < MAJOR_CHAR_MAX; i++)
		init_hlist_head(&chrdev_hash[i]);
}

module_init(chrdev_pure_sync_init, LEVEL_PURE_SYNC);
