/*
 * xboot/kernel/command/cmd_reboot.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <configs.h>
#include <version.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <fs/fsapi.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_REBOOT) && (CONFIG_COMMAND_REBOOT > 0)

static x_s32 reboot(x_s32 argc, const x_s8 **argv)
{
	sync();

	if(reset())
	{
		return 0;
	}
	else
	{
		printk(" the machine does not support 'reboot'\r\n");
		return -1;
	}
}

static struct command reboot_cmd = {
	.name		= "reboot",
	.func		= reboot,
	.desc		= "reboot the target system\r\n",
	.usage		= "reboot\r\n",
	.help		= "    reboot ignores any command line parameters that may be present.\r\n"
};

static __init void reboot_cmd_init(void)
{
	if(!command_register(&reboot_cmd))
		LOG_E("register 'reboot' command fail");
}

static __exit void reboot_cmd_exit(void)
{
	if(!command_unregister(&reboot_cmd))
		LOG_E("unregister 'reboot' command fail");
}

module_init(reboot_cmd_init, LEVEL_COMMAND);
module_exit(reboot_cmd_exit, LEVEL_COMMAND);

#endif
