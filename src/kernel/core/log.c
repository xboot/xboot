/*
 * xboot/kernel/core/log.c
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
#include <types.h>
#include <macros.h>
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/log.h>


/* the list of log */
static struct log_list __log_list = {
	.entry = {
		.next	= &(__log_list.entry),
		.prev	= &(__log_list.entry),
	},
};
static struct log_list * log_list = &__log_list;

/* log format buffer */
static x_s8 log_format_buffer[SZ_4K];

/*
 * get the number of log.
 */
x_s32 log_numberof(void)
{
	x_s32 i = 0;
	struct list_head * pos = (&log_list->entry)->next;

	while(!list_is_last(pos, (&log_list->entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}

/*
 * add a new log into log_list
 */
x_bool log_add(enum log_level level, const x_s32 line, const char * file, const char * fmt, ...)
{
	struct log_list * list;
	va_list args;
	x_s8 * msg;

	list = malloc(sizeof(struct log_list));
	if(!list)
		return FALSE;

	va_start(args, fmt);
	vsnprintf((x_s8 *)log_format_buffer, sizeof(log_format_buffer), (x_s8 *)fmt, args);
	va_end(args);

	msg = malloc(strlen(log_format_buffer));
	if(!msg)
	{
		free(list);
		return FALSE;
	}
	strcpy(msg, log_format_buffer);

	list->level = level;
	list->line = line;
	list->file = (x_s8 *)file;
	list->message = msg;

	if(log_numberof() >= CONFIG_MAX_NUMBER_OF_LOG)
		log_remove();

	list_add(&list->entry, &log_list->entry);

	return TRUE;
}

/*
 * remove the last of log from log_list
 */
x_bool log_remove(void)
{
	struct log_list * list;
	struct list_head * pos = (&log_list->entry)->prev;

	if(!list_empty(&log_list->entry))
	{
		list = list_entry(pos, struct log_list, entry);
		list_del(pos);
		free(list->message);
		free(list);

		return TRUE;
	}

	return FALSE;
}

/*
 * get the log list
 */
struct log_list * get_log_list(void)
{
	return log_list;
}
