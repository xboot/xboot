/*
 * kernel/command/cmd_md.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <string.h>
#include <vsprintf.h>
#include <configs.h>
#include <version.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/ctrlc.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_MC) && (CONFIG_COMMAND_MC > 0)

static x_s32 mc(x_s32 argc, const x_s8 **argv)
{
	x_u32 src, dst, size = 0;
	x_u32 i;

	if(argc != 4)
	{
		printk("usage:\r\n    mc <src> <dst> <size>\r\n");
		return (-1);
	}

	src = simple_strtou32(argv[1], NULL, 0);
	dst = simple_strtou32(argv[2], NULL, 0);
	size = simple_strtou32(argv[3], NULL, 0);

	for(i = 0; i < size; i++)
	{
		*((x_u8 *)(dst+i)) = *((x_u8 *)(src+i));

		if(ctrlc())
			return -1;
	}

	return 0;
}

static struct command mc_cmd = {
	.name		= "mc",
	.func		= mc,
	.desc		= "memory copy\r\n",
	.usage		= "mc <src> <dst> <size>\r\n",
	.help		= "    the number of bytes specified by <size> are copied from <src> to <dst>.\r\n"
				  "    both the source and destination can be located anywhere in the memory address space.\r\n"
	    		  "    note that don't operate system memory (code and work ram and environment etc).\n"
};

static __init void mc_cmd_init(void)
{
	if(!command_register(&mc_cmd))
		LOG_E("register 'mc' command fail");
}

static __exit void mc_cmd_exit(void)
{
	if(!command_unregister(&mc_cmd))
		LOG_E("unregister 'mc' command fail");
}

module_init(mc_cmd_init, LEVEL_COMMAND);
module_exit(mc_cmd_exit, LEVEL_COMMAND);

#endif
