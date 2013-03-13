/*
 * kernel/core/initcall.c
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
#include <xboot/initcall.h>

extern initcall_t __initcall_start[];
extern initcall_t __initcall_end[];
extern exitcall_t __exitcall_start[];
extern exitcall_t __exitcall_end[];

/*
 * do all initial calls
 */
void do_initcalls(void)
{
	initcall_t *call;

	call =  &(*__initcall_start);
	while (call < &(*__initcall_end))
	{
		(*call)();
		call++;
	}
}

/*
 * do all exit calls
 */
void do_exitcalls(void)
{
	exitcall_t *call;

	call =  &(*__exitcall_start);
	while (call < &(*__exitcall_end))
	{
		(*call)();
		call++;
	}
}
