/*
 * drivers/disk/partition.c
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
#include <disk/disk.h>
#include <disk/partition.h>

struct partition_parser_list_t
{
	struct partition_parser_t * parser;
	struct list_head entry;
};

static struct partition_parser_list_t __partition_parser_list = {
	.entry = {
		.next	= &(__partition_parser_list.entry),
		.prev	= &(__partition_parser_list.entry),
	},
};
static spinlock_t __partition_parser_list_lock = SPIN_LOCK_INIT();

static struct partition_parser_t * search_partition_parser(const char * name)
{
	struct partition_parser_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__partition_parser_list.entry), entry)
	{
		if(strcmp(pos->parser->name, name) == 0)
			return pos->parser;
	}

	return NULL;
}

bool_t register_partition_parser(struct partition_parser_t * parser)
{
	struct partition_parser_list_t * ppl;

	if(!parser || !parser->name)
		return FALSE;

	if(!parser->probe)
		return FALSE;

	if(search_partition_parser(parser->name))
		return FALSE;

	ppl = malloc(sizeof(struct partition_parser_list_t));
	if(!ppl)
		return FALSE;

	ppl->parser = parser;

	spin_lock_irq(&__partition_parser_list_lock);
	list_add_tail(&ppl->entry, &(__partition_parser_list.entry));
	spin_unlock_irq(&__partition_parser_list_lock);

	return TRUE;
}

bool_t unregister_partition_parser(struct partition_parser_t * parser)
{
	struct partition_parser_list_t * pos, * n;

	if(!parser || !parser->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__partition_parser_list.entry), entry)
	{
		if(pos->parser == parser)
		{
			spin_lock_irq(&__partition_parser_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__partition_parser_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t partition_parser_probe(struct disk_t * disk)
{
	struct partition_parser_list_t * pos, * n;
	struct partition_t * part;
	struct list_head * part_pos;
	s32_t i;

	if(!disk || !disk->name)
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sectors) || (!disk->write_sectors))
		return FALSE;

	/*
	 * Add partition information for all space of disk
	 */
	init_list_head(&(disk->info.entry));

	part = malloc(sizeof(struct partition_t));
	if(!part)
		return FALSE;

	strlcpy(part->name, "total", sizeof(part->name));
	part->sector_from = 0;
	part->sector_to = disk->sector_count - 1;
	part->sector_size = disk->sector_size;
	part->dev = NULL;
	list_add_tail(&part->entry, &(disk->info.entry));

	/*
	 * Parser partition information
	 */
	list_for_each_entry_safe(pos, n, &(__partition_parser_list.entry), entry)
	{
		if(pos->parser->probe)
		{
			if((pos->parser->probe(disk)) == TRUE)
				break;
		}
	}

	for(i = 0, part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); i++, part_pos = part_pos->next)
	{
		part = list_entry(part_pos, struct partition_t, entry);
		if(i != 0)
		{
			if(strnlen(part->name, sizeof(part->name)) <= 0)
				snprintf(part->name, sizeof(part->name), "part%d", i);
		}
	}

	return TRUE;
}
