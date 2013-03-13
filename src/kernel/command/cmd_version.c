/*
 * kernel/command/cmd_version.c
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
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <console/console.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_VERSION) && (CONFIG_COMMAND_VERSION > 0)

static int version(int argc, char ** argv)
{
	xboot_banner(get_console_stdout());

	return 0;
}

static struct command version_cmd = {
	.name		= "version",
	.func		= version,
	.desc		= "show xboot's version\r\n",
	.usage		= "version\r\n",
	.help		= "    version ignores any command line parameters that may be present.\r\n"
};

static __init void version_cmd_init(void)
{
	if(!command_register(&version_cmd))
		LOG_E("register 'version' command fail");
}

static __exit void version_cmd_exit(void)
{
	if(!command_unregister(&version_cmd))
		LOG_E("unregister 'version' command fail");
}

command_initcall(version_cmd_init);
command_exitcall(version_cmd_exit);

#endif
