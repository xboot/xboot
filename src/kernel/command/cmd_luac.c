/*
 * kernel/command/cmd_luac.c
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
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_LUAC) && (CONFIG_COMMAND_LUAC > 0)

extern int luac_main(int argc, char ** argv);

static int do_luac(int argc, char ** argv)
{
	return luac_main(argc, argv);
}

static struct command luac_cmd = {
	.name		= "luac",
	.func		= do_luac,
	.desc		= "lua compiler\r\n",
	.usage		= "luac [options] [filenames]\r\n",
	.help		= "    available options are:\r\n"
				  "    -        process stdin\r\n"
				  "    -l       list\r\n"
				  "    -o name  output to file 'name' (default is 'luac.out')\r\n"
				  "    -p       parse only\r\n"
				  "    -s       strip debug information\r\n"
				  "    -v       show version information\r\n"
				  "    --       stop handling options\r\n"
};

static __init void luac_cmd_init(void)
{
	if(!command_register(&luac_cmd))
		LOG_E("register 'luac' command fail");
}

static __exit void luac_cmd_exit(void)
{
	if(!command_unregister(&luac_cmd))
		LOG_E("unregister 'luac' command fail");
}

command_initcall(luac_cmd_init);
command_exitcall(luac_cmd_exit);

#endif
