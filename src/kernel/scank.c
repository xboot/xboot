/*
 * kernel/scank.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <time/tick.h>
#include <time/timer.h>
#include <shell/readline.h>
#include <console/console.h>
#include <xboot/scank.h>

/*
 * get a unicode character, ucs-4 format
 */
x_bool getcode(x_u32 * code)
{
	struct console * stdin = get_stdin();

	if(stdin && stdin->getcode)
		return stdin->getcode(stdin, code);
	return FALSE;
}

/*
 * get a unicode character with timeout (x1ms), ucs-4 format
 */
x_bool getcode_with_timeout(x_u32 * code, x_u32 timeout)
{
	struct console * stdin = get_stdin();
	x_u32 end;

	if(!stdin || !stdin->getcode)
		return FALSE;

	if(get_system_hz() > 0)
	{
		end = jiffies + timeout * get_system_hz() / 1000;

		while(! stdin->getcode(stdin, code))
		{
			if(jiffies >= end)
				return FALSE;
		}

		return TRUE;
	}
	else
	{
		end = timeout * 100;

		while(! stdin->getcode(stdin, code))
		{
			if(end <= 0)
				return FALSE;
			end--;
		}

		return TRUE;
	}

	return FALSE;
}

/*
 * scank - unformat input utf-8 stream into a list of arguments
 */
x_s32 scank(const char * fmt, ...)
{
	va_list args;
	x_s8 * p;
	x_s32 i;

	p = readline(0);

	va_start(args,fmt);
	i = vsscanf(p, (x_s8 *)fmt, args);
	va_end(args);

	free(p);
	return i;
}
