/*
 * kernel/command/cmd_console.c
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
#include <command/command.h>


#if	defined(CONFIG_COMMAND_CONSOLE) && (CONFIG_COMMAND_CONSOLE > 0)

static int console(int argc, char ** argv)
{
	if(argc != 2)
		return 0;

	if(strcmp(argv[1], "on") == 0)
	{
		console_onoff(get_console_stdin(), TRUE);
		console_onoff(get_console_stdout(), TRUE);
		console_onoff(get_console_stderr(), TRUE);
	}
	else if(strcmp(argv[1], "off") == 0)
	{
		console_onoff(get_console_stdin(), FALSE);
		console_onoff(get_console_stdout(), FALSE);
		console_onoff(get_console_stderr(), FALSE);
	}

	return 0;
}

static struct command console_cmd = {
	.name		= "console",
	.func		= console,
	.desc		= "turn on/off the standard console\r\n",
	.usage		= "console <on|off>\r\n",
	.help		= "    turn on/off the stdin,stdout and stderr console\r\n"
				  "    the options as below for details\r\n"
				  "    on      turn on the standard console\r\n"
				  "    off     turn off the standard console\r\n"
};

static __init void console_cmd_init(void)
{
	if(!command_register(&console_cmd))
		LOG_E("register 'console' command fail");
}

static __exit void console_cmd_exit(void)
{
	if(!command_unregister(&console_cmd))
		LOG_E("unregister 'console' command fail");
}

command_initcall(console_cmd_init);
command_exitcall(console_cmd_exit);

#endif
