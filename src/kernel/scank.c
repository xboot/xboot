/*
 * kernel/scank.c
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
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <time/tick.h>
#include <time/timer.h>
#include <shell/readline.h>
#include <terminal/terminal.h>
#include <xboot/scank.h>


extern struct hlist_head stdout_list;
extern struct hlist_head stdin_list;
extern void comm_trigger_activity(void);

/*
 * scank -	unformat input stream into a list of arguments
 * @fmt:	formatting of buffer
 * @...:	resulting arguments
 */
x_s32 scank(const char * fmt, ...)
{
	va_list args;
	x_s32 i;

	va_start(args,fmt);
	i = vsscanf(readline(0), (x_s8 *)fmt, args);
	va_end(args);

	return i;
}

/*
 * getch - 	get a char from input stream
 * c:		the point of character
 */
x_bool getch(char * c)
{
	struct terminal_stdin_list * list;
	struct hlist_node * pos;

	hlist_for_each_entry(list,  pos, &stdin_list, node)
	{
		if(list->read(list->term, (x_u8 *)c, 1))
		{
			comm_trigger_activity();
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get a char from input stream and with timeout (x1ms).
 */
x_bool getch_with_timeout(char * c, x_u32 timeout)
{
	char t;
	x_u32 end;

	if(get_system_hz() > 0)
	{
		end = jiffies + timeout * get_system_hz() / 1000;

		while(!getch(&t))
		{
			if(jiffies >= end)
				return FALSE;
		}

		*c = t;
		return TRUE;
	}
	else
	{
		end = timeout * 100;

		while(!getch(&t))
		{
			if(end == 0)
				return FALSE;
			end--;
		}

		*c = t;
		return TRUE;
	}

	return FALSE;
}
