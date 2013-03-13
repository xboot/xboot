/*
 * xboot/kernel/core/log.c
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
#include <types.h>
#include <stddef.h>
#include <sizes.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
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
static char log_format_buffer[SZ_4K];

/*
 * get the number of log.
 */
s32_t log_numberof(void)
{
	s32_t i = 0;
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
bool_t log_add(enum log_level level, const int line, const char * file, const char * fmt, ...)
{
	struct log_list * list;
	va_list args;
	char * msg;

	list = malloc(sizeof(struct log_list));
	if(!list)
		return FALSE;

	va_start(args, fmt);
	vsnprintf(log_format_buffer, sizeof(log_format_buffer), fmt, args);
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
	list->file = (char *)file;
	list->message = msg;

	if(log_numberof() >= CONFIG_MAX_NUMBER_OF_LOG)
		log_remove();

	list_add_tail(&list->entry, &log_list->entry);

	return TRUE;
}

/*
 * remove the last of log from log_list
 */
bool_t log_remove(void)
{
	struct log_list * list;
	struct list_head * pos = (&log_list->entry)->next;

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
