/*
 * kernel/command/cmd-echo.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
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
