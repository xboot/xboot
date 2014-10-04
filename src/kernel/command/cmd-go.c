/*
 * xboot/kernel/command/cmd-go.c
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
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <version.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_GO) && (CONFIG_COMMAND_GO > 0)

static int go(int argc, char ** argv)
{
	u32_t addr;
	s32_t ret;

	if(argc < 2)
	{
		printf("usage:\r\n    go address [arg ...]\r\n");
		return (-1);
	}

	addr = strtoul((const char *)argv[1], NULL, 0);

	printf("starting appligoion at 0x%08lx ...\r\n", addr);

	ret = ((int(*)(int, char **))(addr)) (--argc, &argv[1]);

	printf("application terminated.(ret = 0x%08lx)\r\n", ret);

	return ret;
}

static struct command_t go_cmd = {
	.name		= "go",
	.func		= go,
	.desc		= "start application at address\r\n",
	.usage		= "go address [arg ...]\r\n",
	.help		= "    start application at address.\r\n"
				  "    passing 'arg' as application's arguments.\r\n"
};

static __init void go_cmd_init(void)
{
	if(command_register(&go_cmd))
		LOG("Register command 'go'");
	else
		LOG("Failed to register command 'go'");
}

static __exit void go_cmd_exit(void)
{
	if(command_unregister(&go_cmd))
		LOG("Unegister command 'go'");
	else
		LOG("Failed to unregister command 'cat'");
}

command_initcall(go_cmd_init);
command_exitcall(go_cmd_exit);

#endif
