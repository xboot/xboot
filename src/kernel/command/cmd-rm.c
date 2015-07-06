/*
 * kernel/command/cmd-rm.c
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    rm [OPTION] FILE...\r\n");
}

static int do_rm(int argc, char ** argv)
{
	s32_t i;
	s32_t ret;
	struct stat st;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
	    if(stat((const char*)argv[i], &st) == 0)
	    {
	        if(S_ISDIR(st.st_mode))
	            ret = rmdir((const char*)argv[i]);
	        else
	            ret = unlink((const char*)argv[i]);
			if(ret != 0)
				printf("rm: cannot remove %s: No such file or directory\r\n", argv[i]);
	    }
	    else
	    	printf("rm: cannot stat file or directory %s\r\n", argv[i]);
	}

	return 0;
}

static struct command_t cmd_rm = {
	.name	= "rm",
	.desc	= "remove files or directories",
	.usage	= usage,
	.exec	= do_rm,
};

static __init void rm_cmd_init(void)
{
	command_register(&cmd_rm);
}

static __exit void rm_cmd_exit(void)
{
	command_unregister(&cmd_rm);
}

command_initcall(rm_cmd_init);
command_exitcall(rm_cmd_exit);
