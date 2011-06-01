/*
 * kernel/printk.c
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

#include <xboot.h>
#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <sizes.h>
#include <stdio.h>
#include <malloc.h>
#include <charset.h>
#include <time/tick.h>
#include <time/timer.h>
#include <console/console.h>
#include <xboot/printk.h>

/*
 * printk - Format a string, using utf-8 stream
 */
int printk(const char * fmt, ...)
{
	va_list ap;
	char *p, *buf;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	if(len < 0)
		return 0;
	buf = malloc(len + 1);
	if(!buf)
		return 0;
	len = vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);

	p = buf;
	len = 0;
	while(*p != '\0')
	{
		if(!console_stdout_putc(*p))
			break;
		p++;
		len++;
	}

	free(buf);
	return len;
}



/*
 * get a unicode character, ucs-4 format
 */
bool_t getcode(u32_t * code)
{
	struct console * in = get_console_stdin();

	if(in && in->getcode)
		return in->getcode(in, code);
	return FALSE;
}

/*
 * get a unicode character with timeout (x1ms), ucs-4 format
 */
bool_t getcode_with_timeout(u32_t * code, u32_t timeout)
{
	struct console * in = get_console_stdin();
	u32_t end;

	if(!in || !in->getcode)
		return FALSE;

	if(get_system_hz() > 0)
	{
		end = jiffies + timeout * get_system_hz() / 1000;

		while(! in->getcode(in, code))
		{
			if(jiffies >= end)
				return FALSE;
		}

		return TRUE;
	}
	else
	{
		end = timeout * 100;

		while(! in->getcode(in, code))
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
bool_t stdin_getc(char * c)
{
	u32_t code;

	if(!getcode(&code))
	{
		*c = 0;
		return FALSE;
	}

	c = code & 0xff;
	return TRUE;
}
*/
