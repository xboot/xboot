/*
 * kernel/command/cmd_lua.c
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


#if	defined(CONFIG_COMMAND_LUA) && (CONFIG_COMMAND_LUA > 0)

extern int lua_main(int argc, char ** argv);

static int do_lua(int argc, char ** argv)
{
	return lua_main(argc, argv);
}

static struct command lua_cmd = {
	.name		= "lua",
	.func		= do_lua,
	.desc		= "the stand-alone lua interpreter\r\n",
	.usage		= "lua [options] [script [args]]\r\n",
	.help		= "    available options are:\r\n"
				  "    -e stat  execute string 'stat'\r\n"
				  "    -l name  require library 'name'\r\n"
				  "    -i       enter interactive mode after executing 'script'\r\n"
				  "    -v       show version information\r\n"
				  "    --       stop handling options\r\n"
				  "    -        execute stdin and stop handling options\r\n"
};

static __init void lua_cmd_init(void)
{
	if(!command_register(&lua_cmd))
		LOG_E("register 'lua' command fail");
}

static __exit void lua_cmd_exit(void)
{
	if(!command_unregister(&lua_cmd))
		LOG_E("unregister 'lua' command fail");
}

command_initcall(lua_cmd_init);
command_exitcall(lua_cmd_exit);

#endif
