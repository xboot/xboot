/*
 * kernel/core/syscall.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <macros.h>
#include <fs/fsapi.h>
#include <xboot/syscall.h>

/*
 * the struct of software interrupt registers
 */
struct swi_regs {
	int reg[10];
};

/*
 * system call dispatcher
 */
int syscall_handler(int swi, struct swi_regs * regs)
{
	if(swi != 0)
		return 0;

	switch(regs->reg[0])
	{
	case SYSCALL_FILESYSTEM_SYNC:
		sync();
		break;

	default:
		break;
	}

	return 0;
}
