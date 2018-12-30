/*
 * kernel/command/cmd-help.c
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
	printf("    help [command ...]\r\n");
}

static int do_help(int argc, char ** argv)
{
	struct command_t * pos, * n;
	struct command_t * c;
	struct slist_t * sl, * e;
	int i, j, k = 0;

	if(argc == 1)
	{
		sl = slist_alloc();
		list_for_each_entry_safe(pos, n, &__command_list, list)
		{
			j = strlen(pos->name);
			if(j > k)
				k = j;
			slist_add(sl, pos, "%s", pos->name);
		}
		slist_sort(sl);
		slist_for_each_entry(e, sl)
		{
			pos = (struct command_t *)e->priv;
			printf(" %s%*s - %s\r\n", pos->name, k - strlen(pos->name), "", pos->desc);
		}
		slist_free(sl);
	}
	else
	{
		for(i = 1; i < argc; i++)
		{
			c = search_command(argv[i]);
			if(c)
			{
				printf("%s - %s\r\n", c->name, c->desc);
				if(c->usage)
					c->usage();
			}
			else
			{
				printf("unknown command '%s' - try 'help' for list all of commands\r\n", argv[i]);
			}
		}
	}
	return 0;
}

static struct command_t cmd_help = {
	.name	= "help",
	.desc	= "show online help about command",
	.usage	= usage,
	.exec	= do_help,
};

static __init void help_cmd_init(void)
{
	register_command(&cmd_help);
}

static __exit void help_cmd_exit(void)
{
	unregister_command(&cmd_help);
}

command_initcall(help_cmd_init);
command_exitcall(help_cmd_exit);
