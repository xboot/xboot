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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <macros.h>
#include <vsprintf.h>
#include <malloc.h>
#include <charset.h>
#include <console/console.h>
#include <xboot/printk.h>

extern void led_console_trigger_activity(void);

/*
 * put a unicode character, ucs-4 format
 */
bool_t putcode(u32_t code)
{
	struct console * stdout = get_stdout();

	if(stdout && stdout->putcode)
	{
		led_console_trigger_activity();

		return stdout->putcode(stdout, code);
	}

	return FALSE;
}

/*
 * put a utf-8 character, c is one byte of a utf-8 stream
 *
 * this function gathers bytes until a valid Unicode character is found
 */
void putch(char c)
{
	struct console * stdout = get_stdout();
	static s32_t size = 0;
	static s8_t buf[6];
	s8_t * rest;
	u32_t code;

	if(!stdout || !stdout->putcode)
		return;

	buf[size++] = c;
	while(utf8_to_ucs4(&code, 1, buf, size, (const s8_t **)&rest) > 0)
	{
		led_console_trigger_activity();

		size -= rest - buf;
		memmove(buf, rest, size);
		stdout->putcode(stdout, code);
	}
}

/*
 * printk - Format a string, using utf-8 stream
 */
s32_t printk(const char * fmt, ...)
{
	struct console * stdout = get_stdout();
	va_list args;
	u32_t code;
	s32_t i;
	s8_t *p, *buf;

	if(!stdout || !stdout->putcode)
		return 0;

	buf = malloc(SZ_4K);
	if(!buf)
		return 0;

	va_start(args, fmt);
	i = vsnprintf((s8_t *)buf, SZ_4K, (s8_t *)fmt, args);
	va_end(args);

	led_console_trigger_activity();

	for(p = buf; utf8_to_ucs4(&code, 1, p, -1, (const s8_t **)&p) > 0; )
	{
		stdout->putcode(stdout, code);
	}

	free(buf);
	return i;
}
