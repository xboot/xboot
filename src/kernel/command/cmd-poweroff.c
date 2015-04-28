/*
 * xboot/kernel/command/cmd-shutdown.c
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
#include <command/command.h>

#if	defined(CONFIG_COMMAND_POWEROFF) && (CONFIG_COMMAND_POWEROFF > 0)

static int do_poweroff(int argc, char ** argv)
{
	sync();

	if(machine_poweroff())
	{
		return 0;
	}
	else
	{
		printf(" The machine does not support 'poweroff'\r\n");
		return -1;
	}
}

static struct command_t poweroff_cmd = {
	.name		= "poweroff",
	.func		= do_poweroff,
	.desc		= "poweroff the target system\r\n",
	.usage		= "poweroff\r\n",
	.help		= "    poweroff ignores any command line parameters that may be present.\r\n"
};

static __init void poweroff_cmd_init(void)
{
	if(command_register(&poweroff_cmd))
		LOG("Register command 'poweroff'");
	else
		LOG("Failed to register command 'poweroff'");
}

static __exit void poweroff_cmd_exit(void)
{
	if(command_unregister(&poweroff_cmd))
		LOG("Unegister command 'poweroff'");
	else
		LOG("Failed to unregister command 'poweroff'");
}

command_initcall(poweroff_cmd_init);
command_exitcall(poweroff_cmd_exit);

#endif
