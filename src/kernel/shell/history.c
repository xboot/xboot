/*
 * kernel/shell/history.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <xboot/list.h>
#include <xboot/printk.h>
#include <shell/history.h>


/* the list of history */
static struct history_list __history_list = {
	.entry = {
		.next	= &(__history_list.entry),
		.prev	= &(__history_list.entry),
	},
};
static struct history_list * history_list = &__history_list;
static struct history_list * history_current = &__history_list;

/*
 * add the command line to history list
 * return true is successed, otherwise is not.
 */
x_bool history_add(const x_s8 * cmdline)
{
	x_s8 * s;
	struct history_list * list;
	struct list_head * pos;

	history_current = history_list;
	if(strlen(cmdline) == 0)
		return FALSE;

	pos = (&history_list->entry)->next;
	list = list_entry(pos, struct history_list, entry);
	if(strcmp(list->cmdline, cmdline) == 0)
		return TRUE;

	list = malloc(sizeof(struct history_list));
	if(!list || !cmdline)
	{
		free(list);
		return FALSE;
	}

	s = malloc(strlen(cmdline));
	if(!s)
	{
		free(s);
		free(list);
		return FALSE;
	}

	if(history_numberof() >= CONFIG_NUMBER_OF_CMDLINE_HISTORY)
		history_remove();

	strcpy(s, cmdline);
	list->cmdline = s;
	list_add(&list->entry, &history_list->entry);

	return TRUE;
}

/*
 * remove the last of command line from history list
 */
x_bool history_remove(void)
{
	struct history_list * list, * list_prev;
	struct list_head * pos = (&history_list->entry)->prev;

	if(!list_empty(&history_list->entry))
	{
		list = list_entry(pos, struct history_list, entry);
		if(history_current == list)
		{
			list_prev = list_entry((&history_current->entry)->prev, struct history_list, entry);
			if(list_prev != history_list)
				history_current = list_prev;
			else
				history_current = NULL;
		}
		list_del(pos);
		free(list->cmdline);
		free(list);
		return TRUE;
	}

	return FALSE;
}

/*
 * get the number of history.
 */
x_s32 history_numberof(void)
{
	x_s32 i = 0;
	struct list_head * pos = (&history_list->entry)->next;

	while(!list_is_last(pos, (&history_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}

/*
 * get the history list
 */
struct history_list * get_history_list(void)
{
	return history_list;
}

/*
 * get the next command line.
 */
x_s8 * history_get_next_cmd(void)
{
	struct history_list * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	if(history_current == history_list)
		return NULL;

	list = list_entry((&history_current->entry)->prev, struct history_list, entry);
	history_current = list;
	if(list != history_list)
		return history_current->cmdline;
	else
		return NULL;
}

/*
 * get the prev command line.
 */
x_s8 * history_get_prev_cmd(void)
{
	struct history_list * list;

	if(list_empty(&history_list->entry))
	{
		history_current = history_list;
		return NULL;
	}

	list = list_entry((&history_current->entry)->next, struct history_list, entry);
	if(list != history_list)
	{
		history_current = list;
		return history_current->cmdline;
	}
	else
		return NULL;
}
