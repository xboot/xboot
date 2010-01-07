/*
 * xboot/kernel/command/cmd_halt.c
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
#include <debug.h>
#include <string.h>
#include <configs.h>
#include <version.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <shell/command.h>


#if	defined(CONFIG_COMMAND_HALT) && (CONFIG_COMMAND_HALT > 0)

static x_s32 do_halt(x_s32 argc, const x_s8 **argv)
{
	if(halt())
		return 0;
	else
	{
		printk(" this machine not support 'halt'\r\n");
		return -1;
	}
}

static struct command halt_cmd = {
	.name		= "halt",
	.func		= do_halt,
	.desc		= "halt the target system\r\n",
	.usage		= "halt\r\n",
	.help		= "    halt ignores any command line parameters that may be present.\r\n"
};

static __init void halt_cmd_init(void)
{
	if(!command_register(&halt_cmd))
		DEBUG_E("register 'halt' command fail");
}

static __exit void halt_cmd_exit(void)
{
	if(!command_unregister(&halt_cmd))
		DEBUG_E("unregister 'halt' command fail");
}

module_init(halt_cmd_init, LEVEL_COMMAND);
module_exit(halt_cmd_exit, LEVEL_COMMAND);

#endif
