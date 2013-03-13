/*
 * xboot/kernel/command/cmd_halt.c
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
#include <string.h>
#include <version.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <fs/fileio.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_HALT) && (CONFIG_COMMAND_HALT > 0)

static int do_halt(int argc, char ** argv)
{
	sync();

	if(machine_halt())
	{
		return 0;
	}
	else
	{
		printk(" the machine does not support 'halt'\r\n");
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
		LOG_E("register 'halt' command fail");
}

static __exit void halt_cmd_exit(void)
{
	if(!command_unregister(&halt_cmd))
		LOG_E("unregister 'halt' command fail");
}

command_initcall(halt_cmd_init);
command_exitcall(halt_cmd_exit);

#endif
