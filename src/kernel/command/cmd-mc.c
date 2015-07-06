/*
 * kernel/command/cmd-md.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("Usage:\r\n");
	printf("    mc <src> <dst> <size>\r\n");
}

static int do_mc(int argc, char ** argv)
{
	u32_t src, dst, size = 0;
	u32_t i;

	if(argc != 4)
	{
		usage();
		return -1;
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

static struct command_t cmd_mc = {
	.name	= "mc",
	.desc	= "memory copy",
	.usage	= usage,
	.exec	= do_mc,
};

static __init void mc_cmd_init(void)
{
	command_register(&cmd_mc);
}

static __exit void mc_cmd_exit(void)
{
	command_unregister(&cmd_mc);
}

command_initcall(mc_cmd_init);
command_exitcall(mc_cmd_exit);
