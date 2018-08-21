/*
 * driver/block/partition/partition.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
