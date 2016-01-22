/*
 * kernel/command/cmd-rmdir.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    rmdir DIRECTORY...\r\n");
}

static int do_rmdir(int argc, char ** argv)
{
	s32_t ret = 0;
	s32_t i;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(rmdir((const char*)argv[i]) != 0)
		{
			ret = -1;
			printf("mkdir: failed to remove directory %s\r\n", argv[i]);
		}
	}

	return ret;
}

static struct command_t cmd_rmdir = {
	.name	= "rmdir",
	.desc	= "remove empty directories",
	.usage	= usage,
	.exec	= do_rmdir,
};

static __init void rmdir_cmd_init(void)
{
	register_command(&cmd_rmdir);
}

static __exit void rmdir_cmd_exit(void)
{
	unregister_command(&cmd_rmdir);
}

command_initcall(rmdir_cmd_init);
command_exitcall(rmdir_cmd_exit);
