/*
 * kernel/command/cmd-rmdir.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_RMDIR) && (CONFIG_COMMAND_RMDIR > 0)

static int do_rmdir(int argc, char ** argv)
{
	s32_t ret = 0;
	s32_t i;

	if(argc < 2)
	{
		printf("usage:\r\n    rmdir DIRECTORY...\r\n");
		return (-1);
	}

	for(i=1; i<argc; i++)
	{
		if(rmdir((const char*)argv[i]) != 0)
		{
			ret = -1;
			printf("mkdir: failed to remove directory %s\r\n", argv[i]);
		}
	}

	return ret;
}

static struct command_t rmdir_cmd = {
	.name		= "rmdir",
	.func		= do_rmdir,
	.desc		= "remove empty directories\r\n",
	.usage		= "rmdir DIRECTORY...\r\n",
	.help		= "    remove the DIRECTORY, if they are empty.\r\n"
};

static __init void rmdir_cmd_init(void)
{
	if(command_register(&rmdir_cmd))
		LOG("Register command 'rmdir'");
	else
		LOG("Failed to register command 'rmdir'");
}

static __exit void rmdir_cmd_exit(void)
{
	if(command_unregister(&rmdir_cmd))
		LOG("Unegister command 'rmdir'");
	else
		LOG("Failed to unregister command 'rmdir'");
}

command_initcall(rmdir_cmd_init);
command_exitcall(rmdir_cmd_exit);

#endif
