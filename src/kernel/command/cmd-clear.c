/*
 * kernel/command/cmd-clear.c
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
#include <console/console.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_CLEAR) && (CONFIG_COMMAND_CLEAR > 0)

static int clear(int argc, char ** argv)
{
	printf("\033[2J");
	return 0;
}

static struct command_t clear_cmd = {
	.name		= "clear",
	.func		= clear,
	.desc		= "clear the terminal screen\r\n",
	.usage		= "clear\r\n",
	.help		= "    clear clears your screen if this is possible\r\n"
				  "    clear ignores any command line parameters that may be present.\r\n"
};

static __init void clear_cmd_init(void)
{
	if(command_register(&clear_cmd))
		LOG("Register command 'clear'");
	else
		LOG("Failed to register command 'clear'");
}

static __exit void clear_cmd_exit(void)
{
	if(command_unregister(&clear_cmd))
		LOG("Unegister command 'clear'");
	else
		LOG("Failed to unregister command 'clear'");
}

command_initcall(clear_cmd_init);
command_exitcall(clear_cmd_exit);

#endif
