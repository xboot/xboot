/*
 * driver/block/partition/partition.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <block/partition.h>

struct partition_map_list_t
{
	struct partition_map_t * map;
	struct list_head entry;
};

static struct partition_map_list_t __partition_map_list = {
	.entry = {
		.next	= &(__partition_map_list.entry),
		.prev	= &(__partition_map_list.entry),
	},
};
static spinlock_t __partition_map_list_lock = SPIN_LOCK_INIT();

static struct partition_map_t * search_partition_map(const char * name)
{
	struct partition_map_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__partition_map_list.entry), entry)
	{
		if(strcmp(pos->map->name, name) == 0)
			return pos->map;
	}

	return NULL;
}

bool_t register_partition_map(struct partition_map_t * map)
{
	struct partition_map_list_t * pml;
	irq_flags_t flags;

	if(!map || !map->name || !map->map)
		return FALSE;

	if(search_partition_map(map->name))
		return FALSE;

	pml = malloc(sizeof(struct partition_map_list_t));
	if(!pml)
		return FALSE;

	pml->map = map;

	spin_lock_irqsave(&__partition_map_list_lock, flags);
	list_add_tail(&pml->entry, &(__partition_map_list.entry));
	spin_unlock_irqrestore(&__partition_map_list_lock, flags);

	return TRUE;
}

bool_t unregister_partition_map(struct partition_map_t * map)
{
	struct partition_map_list_t * pos, * n;
	irq_flags_t flags;

	if(!map || !map->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__partition_map_list.entry), entry)
	{
		if(pos->map == map)
		{
			spin_lock_irqsave(&__partition_map_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__partition_map_list_lock, flags);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t partition_map(struct disk_t * disk)
{
	struct partition_map_list_t * pos, * n;
	struct partition_t * ppos, * pn;
	int i = 0;

	if(!disk || !disk->name)
		return FALSE;

	if(!disk->size || !disk->count)
		return FALSE;

	init_list_head(&(disk->part.entry));
	list_for_each_entry_safe(pos, n, &(__partition_map_list.entry), entry)
	{
		if(pos->map->map)
		{
			if((pos->map->map(disk)) == TRUE)
				break;
		}
	}

	list_for_each_entry_safe(ppos, pn, &(disk->part.entry), entry)
	{
		if(!strlen(ppos->name))
			snprintf(ppos->name, sizeof(ppos->name), "p%d", ++i);
	}

	return TRUE;
}
