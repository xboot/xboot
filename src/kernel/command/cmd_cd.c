/*
 * kernel/command/cmd_cd.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <types.h>
#include <string.h>
#include <debug.h>
#include <malloc.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/command.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_CD) && (CONFIG_COMMAND_CD > 0)

static x_s32 do_cd(x_s32 argc, const x_s8 **argv)
{
	if(argc == 1)
	{
		if(chdir("/") != 0)
		{
			printk("cd: %s: no such directory\r\n", "/");
			return -1;
		}
	}
	else
	{
		if(chdir((char *)argv[1]) != 0)
		{
			printk("cd: %s: no such directory\r\n", argv[1]);
			return -1;
		}
	}

	return 0;
}

static struct command cd_cmd = {
	.name		= "cd",
	.func		= do_cd,
	.desc		= "change the current working directory\r\n",
	.usage		= "cd [DIR]\r\n",
	.help		= "    change the current directory to DIR.\r\n"
				  "    no DIR will change directory to '/'.\r\n"
};

static __init void cd_cmd_init(void)
{
	if(!command_register(&cd_cmd))
		DEBUG_E("register 'cd' command fail");
}

static __exit void cd_cmd_exit(void)
{
	if(!command_unregister(&cd_cmd))
		DEBUG_E("unregister 'cd' command fail");
}

module_init(cd_cmd_init, LEVEL_COMMAND);
module_exit(cd_cmd_exit, LEVEL_COMMAND);

#endif
