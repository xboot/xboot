/*
 * kernel/command/cmd-kill.c
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
	struct window_manager_t * pos, * n;
	struct window_t * wpos, * wn;
	struct slist_t * sl, * e;

	printf("usage:\r\n");
	printf("    kill <name>\r\n");

	printf("task:\r\n");
	sl = slist_alloc();
	list_for_each_entry_safe(pos, n, &__window_manager_list, list)
	{
		list_for_each_entry_safe(wpos, wn, &pos->window, list)
		{
			slist_add(sl, wpos, "%s", wpos->task->name);
		}
	}
	slist_sort(sl);
	slist_for_each_entry(e, sl)
	{
		printf("    %s\r\n", e->key);
	}
	slist_free(sl);
}

static int do_kill(int argc, char ** argv)
{
	struct window_manager_t * pos, * n;
	struct window_t * wpos, * wn;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	list_for_each_entry_safe(pos, n, &__window_manager_list, list)
	{
		list_for_each_entry_safe(wpos, wn, &pos->window, list)
		{
			if(strcmp(argv[1], wpos->task->name) == 0)
			{
				window_exit(wpos);
				return 0;
			}
		}
	}
	printf("No such window task '%s'\r\n", argv[1]);
	return -1;
}

static struct command_t cmd_kill = {
	.name	= "kill",
	.desc	= "kill the window task",
	.usage	= usage,
	.exec	= do_kill,
};

static __init void kill_cmd_init(void)
{
	register_command(&cmd_kill);
}

static __exit void kill_cmd_exit(void)
{
	unregister_command(&cmd_kill);
}

command_initcall(kill_cmd_init);
command_exitcall(kill_cmd_exit);
