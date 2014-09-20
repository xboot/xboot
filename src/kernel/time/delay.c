/*
 * kernel/time/delay.c
 *
 * some function copyed from linux kernel.
 * the calibrate_delay function be write by Linus Torvalds
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

#include <time/delay.h>

static inline bool_t __is_timeout(u64_t start, u64_t offset)
{
	if((int64_t)(start + offset - clocksource_gettime()) < 0)
		return TRUE;
	else
		return FALSE;
}

bool_t is_timeout(u64_t start, u64_t offset)
{
	if(offset >= 100)
	{
		//poller_call();
	}
	return __is_timeout(start, offset);
}
EXPORT_SYMBOL(is_timeout);

void udelay(u32_t us)
{
	u64_t start = clocksource_gettime();
	while(!is_timeout(start, us));
}
EXPORT_SYMBOL(udelay);

void mdelay(u32_t ms)
{
	u64_t start = clocksource_gettime();
	while(!is_timeout(start, ms * (u64_t)1000));
}
EXPORT_SYMBOL(mdelay);
