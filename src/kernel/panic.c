/*
 * kernel/panic.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <stdarg.h>
#include <malloc.h>
#include <vsprintf.h>
#include <time/timer.h>
#include <time/delay.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/panic.h>

/*
 * system panic
 */
void panic(const char * fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	printk(fmt);
	va_end(args);

	printk("system will reboot after 4 seconds\r\n");
	mdelay(4000);

	if(!reset())
	{
		printk("reboot system fail, not support!\r\n");
		while(1);
	}
}
