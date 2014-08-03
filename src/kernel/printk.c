/*
 * kernel/printk.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <xboot/module.h>
#include <xboot/printk.h>

/*
 * printk - Format a string, using utf-8 stream
 */
int printk(const char * fmt, ...)
{
	va_list ap;
	char buf[SZ_4K];
	char *p;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(buf, SZ_4K, fmt, ap);
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

	return len;
}
EXPORT_SYMBOL(printk);
