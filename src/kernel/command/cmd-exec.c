/*
 * kernel/command/cmd-exec.c
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
#include <framework/vm.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_EXEC) && (CONFIG_COMMAND_EXEC > 0)

static void usage(void)
{
	printf("usage:\r\n");
	printf("    exec <file> [arg ...]\r\n");
}

static int exec(int argc, char ** argv)
{
	if(argc < 2)
	{
		usage();
		return -1;
	}

	return vm_exec(argv[1], --argc, &argv[1]);
}

static struct command_t exec_cmd = {
	.name		= "exec",
	.func		= exec,
	.desc		= "execute a file\r\n",
	.usage		= "exec <file> [arg ...]\r\n",
	.help		= "    execute a file\r\n"
};

static __init void exec_cmd_init(void)
{
	if(command_register(&exec_cmd))
		LOG("Register command 'exec'");
	else
		LOG("Failed to register command 'exec'");
}

static __exit void exec_cmd_exit(void)
{
	if(command_unregister(&exec_cmd))
		LOG("Unegister command 'exec'");
	else
		LOG("Failed to unregister command 'exec'");
}

command_initcall(exec_cmd_init);
command_exitcall(exec_cmd_exit);

#endif
