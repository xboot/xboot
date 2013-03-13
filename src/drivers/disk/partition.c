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
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <disk/disk.h>
#include <disk/partition.h>

/* the list of partition parser */
static struct partition_parser_list __partition_parser_list = {
	.entry = {
		.next	= &(__partition_parser_list.entry),
		.prev	= &(__partition_parser_list.entry),
	},
};
static struct partition_parser_list * partition_parser_list = &__partition_parser_list;

/*
 * search partition parser by name
 */
static struct partition_parser * search_partition_parser(const char * name)
{
	struct partition_parser_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&partition_parser_list->entry)->next; pos != (&partition_parser_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct partition_parser_list, entry);
		if(strcmp(list->parser->name, name) == 0)
			return list->parser;
	}

	return NULL;
}

/*
 * register a partition parser into partition_parser_list
 */
bool_t register_partition_parser(struct partition_parser * parser)
{
	struct partition_parser_list * list;

	list = malloc(sizeof(struct partition_parser_list));
	if(!list || !parser)
	{
		free(list);
		return FALSE;
	}

	if(!parser->name || search_partition_parser(parser->name))
	{
		free(list);
		return FALSE;
	}

	list->parser = parser;
	list_add(&list->entry, &partition_parser_list->entry);

	return TRUE;
}

/*
 * unregister partition parser from partition_parser_list
 */
bool_t unregister_partition_parser(struct partition_parser * parser)
{
	struct partition_parser_list * list;
	struct list_head * pos;

	if(!parser || !parser->name)
		return FALSE;

	for(pos = (&partition_parser_list->entry)->next; pos != (&partition_parser_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct partition_parser_list, entry);
		if(list->parser == parser)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * probe partition with parser which can be used
 */
bool_t partition_parser_probe(struct disk * disk)
{
	struct partition_parser_list * list;
	struct list_head * pos;
	struct partition * part;
	struct list_head * part_pos;
	s32_t i;

	if(!disk || !disk->name)
		return FALSE;

	if((disk->sector_size <= 0) || (disk->sector_count <=0))
		return FALSE;

	if((!disk->read_sectors) || (!disk->write_sectors))
		return FALSE;

	/*
	 * add partition information for all space of disk
	 */
	init_list_head(&(disk->info.entry));

	part = malloc(sizeof(struct partition));
	if(!part)
		return FALSE;

	strlcpy(part->name, "TOTAL", sizeof(part->name));
	part->sector_from = 0;
	part->sector_to = disk->sector_count - 1;
	part->sector_size = disk->sector_size;
	part->dev = NULL;
	list_add_tail(&part->entry, &(disk->info.entry));

	/*
	 * parser partition information
	 */
	for(pos = (&partition_parser_list->entry)->next; pos != (&partition_parser_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct partition_parser_list, entry);
		if(list->parser->probe)
		{
			if((list->parser->probe(disk)) == TRUE)
				break;
		}
	}

	for(i = 0, part_pos = (&(disk->info.entry))->next; part_pos != &(disk->info.entry); i++, part_pos = part_pos->next)
	{
		part = list_entry(part_pos, struct partition, entry);
		if(i != 0)
		{
			if(strnlen(part->name, sizeof(part->name)) <= 0)
				snprintf(part->name, sizeof(part->name), "part%ld", i);
		}
	}

	return TRUE;
}
