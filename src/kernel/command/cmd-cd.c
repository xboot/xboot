/*
 * kernel/command/cmd-cd.c
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

#include <xboot.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_CD) && (CONFIG_COMMAND_CD > 0)

static int do_cd(int argc, char ** argv)
{
	if(argc == 1)
	{
		if(chdir("/") != 0)
		{
			printf("cd: %s: No such file or directory\r\n", "/");
			return -1;
		}
	}
	else
	{
		if(chdir(argv[1]) != 0)
		{
			printf("cd: %s: No such file or directory\r\n", argv[1]);
			return -1;
		}
	}

	return 0;
}

static struct command_t cd_cmd = {
	.name		= "cd",
	.func		= do_cd,
	.desc		= "change the current working directory\r\n",
	.usage		= "cd [DIR]\r\n",
	.help		= "    change the current directory to DIR.\r\n"
				  "    no DIR will change directory to '/'.\r\n"
};

static __init void cd_cmd_init(void)
{
	if(command_register(&cd_cmd))
		LOG("Register command 'cd'");
	else
		LOG("Failed to register command 'cd'");
}

static __exit void cd_cmd_exit(void)
{
	if(command_unregister(&cd_cmd))
		LOG("Unegister command 'cd'");
	else
		LOG("Failed to unregister command 'cd'");
}

command_initcall(cd_cmd_init);
command_exitcall(cd_cmd_exit);

#endif
