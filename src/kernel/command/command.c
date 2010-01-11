/*
 * kernel/command/command.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang
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
#include <xboot/log.h>
#include <xboot/list.h>
#include <shell/command.h>

/* the list of command */
static struct command_list __command_list = {
	.entry = {
		.next	= &(__command_list.entry),
		.prev	= &(__command_list.entry),
	},
};
struct command_list * command_list = &__command_list;

/*
 * register a command into command_list
 * return true is successed, otherwise is not.
 */
x_bool command_register(struct command * cmd)
{
	struct command_list * list;

	list = malloc(sizeof(struct command_list));
	if(!list || !cmd)
	{
		free(list);
		return FALSE;
	}

	if(!cmd->name || !cmd->func || command_search(cmd->name))
	{
		free(list);
		return FALSE;
	}

	list->cmd = cmd;
	list_add(&list->entry, &command_list->entry);

	return TRUE;
}

/*
 * unregister command from command_list
 */
x_bool command_unregister(struct command * cmd)
{
	struct command_list * list;
	struct list_head * pos;

	for(pos = (&command_list->entry)->next; pos != (&command_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct command_list, entry);
		if(list->cmd == cmd)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * search command.
 */
struct command * command_search(const char *name)
{
	struct command_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&command_list->entry)->next; pos != (&command_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct command_list, entry);
		if(strcmp((x_s8*)list->cmd->name, (const x_s8 *)name) == 0)
			return list->cmd;
	}

	return NULL;
}

/*
 * the number of commands
 */
x_s32 command_number(void)
{
	x_s32 i = 0;
	struct list_head * pos = (&command_list->entry)->next;

	while(!list_is_last(pos, (&command_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}
