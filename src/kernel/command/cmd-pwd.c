/*
 * kernel/command/cmd-pwd.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/vfs/vfs.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_PWD) && (CONFIG_COMMAND_PWD > 0)

static int do_pwd(int argc, char ** argv)
{
	s8_t cwd[MAX_PATH];

	if(getcwd((char *)cwd, sizeof(cwd)))
	{
		printk("%s\r\n", (char *)cwd);
		return 0;
	}

	return -1;
}

static struct command_t pwd_cmd = {
	.name		= "pwd",
	.func		= do_pwd,
	.desc		= "print the current working directory\r\n",
	.usage		= "pwd\r\n",
	.help		= "    print the current working directory.\r\n"
};

static __init void pwd_cmd_init(void)
{
	if(command_register(&pwd_cmd))
		LOG("Register command 'pwd'");
	else
		LOG("Failed to register command 'pwd'");
}

static __exit void pwd_cmd_exit(void)
{
	if(command_unregister(&pwd_cmd))
		LOG("Unegister command 'pwd'");
	else
		LOG("Failed to unregister command 'pwd'");
}

command_initcall(pwd_cmd_init);
command_exitcall(pwd_cmd_exit);

#endif
