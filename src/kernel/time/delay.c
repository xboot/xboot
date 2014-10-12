/*
 * kernel/time/delay.c
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

#include <xboot.h>
#include <time/delay.h>

bool_t istimeout(u64_t start, u64_t offset)
{
	if((int64_t)(start + offset - clocksource_gettime()) < 0)
		return TRUE;
	return FALSE;
}
EXPORT_SYMBOL(istimeout);

void udelay(u32_t us)
{
	u64_t start = clocksource_gettime();
	u64_t offset = us;
	while(!istimeout(start, offset));
}
EXPORT_SYMBOL(udelay);

void mdelay(u32_t ms)
{
	u64_t start = clocksource_gettime();
	u64_t offset = ms * (u64_t)1000;
	while(!istimeout(start, offset));
}
EXPORT_SYMBOL(mdelay);
