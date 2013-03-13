/*
 * xboot/kernel/command/cmd_go.c
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
#include <command/command.h>


#if	defined(CONFIG_COMMAND_GO) && (CONFIG_COMMAND_GO > 0)

static int go(int argc, char ** argv)
{
	u32_t addr;
	s32_t ret;

	if(argc < 2)
	{
		printk("usage:\r\n    go address [arg ...]\r\n");
		return (-1);
	}

	addr = strtoul((const char *)argv[1], NULL, 0);

	printk("starting application at 0x%08lx ...\r\n", addr);

	ret = ((int(*)(int, char **))(addr)) (--argc, &argv[1]);

	printk("application terminated.(ret = 0x%08lx)\r\n", ret);

	return ret;
}

static struct command go_cmd = {
	.name		= "go",
	.func		= go,
	.desc		= "start application at address\r\n",
	.usage		= "go address [arg ...]\r\n",
	.help		= "    start application at address.\r\n"
				  "    passing 'arg' as application's arguments.\r\n"
};

static __init void go_cmd_init(void)
{
	if(!command_register(&go_cmd))
		LOG_E("register 'go' command fail");
}

static __exit void go_cmd_exit(void)
{
	if(!command_unregister(&go_cmd))
		LOG_E("unregister 'go' command fail");
}

command_initcall(go_cmd_init);
command_exitcall(go_cmd_exit);

#endif
