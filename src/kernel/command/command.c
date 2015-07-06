/*
 * kernel/command/command.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <command/command.h>

static struct command_list_t __command_list = {
	.entry = {
		.next	= &(__command_list.entry),
		.prev	= &(__command_list.entry),
	},
};
struct command_list_t * command_list = &__command_list;

bool_t command_register(struct command_t * cmd)
{
	struct command_list_t * list;

	list = malloc(sizeof(struct command_list_t));
	if(!list || !cmd)
	{
		free(list);
		return FALSE;
	}

	if(!cmd->name || !cmd->exec || command_search(cmd->name))
	{
		free(list);
		return FALSE;
	}

	list->cmd = cmd;
	list_add(&list->entry, &command_list->entry);

	return TRUE;
}

bool_t command_unregister(struct command_t * cmd)
{
	struct command_list_t * list;
	struct list_head * pos;

	for(pos = (&command_list->entry)->next; pos != (&command_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct command_list_t, entry);
		if(list->cmd == cmd)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

struct command_t * command_search(const char * name)
{
	struct command_list_t * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&command_list->entry)->next; pos != (&command_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct command_list_t, entry);
		if(strcmp(list->cmd->name, name) == 0)
			return list->cmd;
	}

	return NULL;
}

int command_number(void)
{
	struct list_head * pos = (&command_list->entry)->next;
	int i = 0;

	while(!list_is_last(pos, (&command_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}
