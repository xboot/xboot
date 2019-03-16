/*
 * kernel/command/cmd-env.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
	printf("    env [name=value] ...\r\n");
}

static int do_env(int argc, char ** argv)
{
	struct environ_t * environ = &__xenviron;
	struct environ_t * p;
	int i;

	for(i = 1; i < argc; i++)
	{
		if(strchr(argv[i], '='))
		{
			putenv(argv[i]);
		}
		else
		{
			usage();
			return -1;
		}
	}

	for(p = environ->next; p != environ; p = p->next)
	{
		printf(" %s\r\n", p->content);
	}

	return 0;
}

static struct command_t cmd_env = {
	.name	= "env",
	.desc	= "display environment variable",
	.usage	= usage,
	.exec	= do_env,
};

static __init void env_cmd_init(void)
{
	register_command(&cmd_env);
}

static __exit void env_cmd_exit(void)
{
	unregister_command(&cmd_env);
}

command_initcall(env_cmd_init);
command_exitcall(env_cmd_exit);
