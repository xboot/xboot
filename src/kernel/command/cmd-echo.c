/*
 * kernel/command/cmd-echo.c
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

#if	defined(CONFIG_COMMAND_ECHO) && (CONFIG_COMMAND_ECHO > 0)

static int echo(int argc, char ** argv)
{
	int nflag = 0;

	if(*++argv && !strcmp(*argv, "-n"))
	{
		++argv;
		nflag = 1;
	}

	while(*argv)
	{
		printf("%s", *argv);
		if(*++argv)
			putchar(' ');
	}

	if(nflag == 0)
		printf("\r\n");
	fflush(stdout);

	return 0;
}

static struct command_t echo_cmd = {
	.name		= "echo",
	.func		= echo,
	.desc		= "display a line of text\r\n",
	.usage		= "echo [OPTION]... [STRING]...\r\n",
	.help		= "    echo the STRING(s) to standard output.\r\n"
				  "    -n    do not output the trailing newline\r\n"
};

static __init void echo_cmd_init(void)
{
	if(command_register(&echo_cmd))
		LOG("Register command 'echo'");
	else
		LOG("Failed to register command 'echo'");
}

static __exit void echo_cmd_exit(void)
{
	if(command_unregister(&echo_cmd))
		LOG("Unegister command 'echo'");
	else
		LOG("Failed to unregister command 'echo'");
}

command_initcall(echo_cmd_init);
command_exitcall(echo_cmd_exit);

#endif
