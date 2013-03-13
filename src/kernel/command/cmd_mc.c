/*
 * kernel/command/cmd_md.c
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
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <version.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/ctrlc.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_MC) && (CONFIG_COMMAND_MC > 0)

static int mc(int argc, char ** argv)
{
	u32_t src, dst, size = 0;
	u32_t i;

	if(argc != 4)
	{
		printk("usage:\r\n    mc <src> <dst> <size>\r\n");
		return (-1);
	}

	src = strtoul((const char *)argv[1], NULL, 0);
	dst = strtoul((const char *)argv[2], NULL, 0);
	size = strtoul((const char *)argv[3], NULL, 0);

	for(i = 0; i < size; i++)
	{
		*((u8_t *)(dst+i)) = *((u8_t *)(src+i));

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

command_initcall(mc_cmd_init);
command_exitcall(mc_cmd_exit);

#endif
