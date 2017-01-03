/*
 * kernel/command/cmd-echo.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    echo [option] [string]...\r\n");
	printf("    -n    do not output the trailing newline\r\n");
}

static int do_echo(int argc, char ** argv)
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

static struct command_t cmd_echo = {
	.name	= "echo",
	.desc	= "echo the string to standard output",
	.usage	= usage,
	.exec	= do_echo,
};

static __init void echo_cmd_init(void)
{
	register_command(&cmd_echo);
}

static __exit void echo_cmd_exit(void)
{
	unregister_command(&cmd_echo);
}

command_initcall(echo_cmd_init);
command_exitcall(echo_cmd_exit);
