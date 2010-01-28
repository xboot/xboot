/*
 * kernel/printk.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
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
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <terminal/terminal.h>

extern struct hlist_head stdout_list;
extern struct hlist_head stdin_list;
extern void comm_trigger_activity(void);

/*
 * log buffer for system printk.
 */
#define	PRINTK_LOG_BUFFER_SIZE		(CONFIG_PRINTK_BUF_SIZE * 2)

static x_s8 log_buf[PRINTK_LOG_BUFFER_SIZE];
static x_u32 log_tail = 0;
static x_u32 log_head = 0;

static void emit_log_char(x_s8 c)
{
	if( (log_tail + PRINTK_LOG_BUFFER_SIZE - log_head) % PRINTK_LOG_BUFFER_SIZE == 1)
	{
		log_tail = (log_tail + 1) % PRINTK_LOG_BUFFER_SIZE;
	}

	log_buf[log_head] = c;
	log_head = (log_head + 1) % PRINTK_LOG_BUFFER_SIZE;
}

static x_u32 pop_log_char(x_s8 *s)
{
	x_u32 i = 0;

	while(!(log_tail==log_head))
	{
		*s = log_buf[log_tail];
		log_tail = (log_tail + 1) % PRINTK_LOG_BUFFER_SIZE;
		s++;
		i++;
	}
	return i;
}

/*
 * printk - Format a string and place it in a log_buf
 * @fmt:	The format string to use
 * @...:	Arguments for the format string
 */
x_s32 printk(const char * fmt, ...)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	va_list args;
	x_s32 i;
	x_s8 * buf = malloc(CONFIG_PRINTK_BUF_SIZE);
	x_s8 * p;

	if(!buf)
		return 0;

	va_start(args, fmt);
	i = vsnprintf((x_s8 *)buf, CONFIG_PRINTK_BUF_SIZE, (x_s8 *)fmt, args);
	va_end(args);

	for (p = buf; *p; p++)
	{
		emit_log_char(*p);
	}

	if(get_stdout_status())
	{
		comm_trigger_activity();

		i = pop_log_char(buf);
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			list->write(list->term, (x_u8 *)buf, i);
		}
	}

	free(buf);
	return i;
}

/*
 * putch - put a char and place it in a log_buf
 * c:	  the character to put
 */
void putch(char c)
{
	struct terminal_stdout_list * list;
	struct hlist_node * pos;
	x_s32 i;
	x_s8 * buf = malloc(CONFIG_PRINTK_BUF_SIZE);

	emit_log_char((x_s8)c);

	if(!buf)
		return;

	if(get_stdout_status())
	{
		comm_trigger_activity();

		i = pop_log_char(buf);
		hlist_for_each_entry(list,  pos, &stdout_list, node)
		{
			list->write(list->term, (x_u8 *)buf, i);
		}
	}

	free(buf);
	return;
}

