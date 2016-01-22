/*
 * kernel/command/cmd-env.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
	printf("    env [NAME=VALUE] ...\r\n");
}

static int do_env(int argc, char ** argv)
{
	struct environ_t * environ = &(runtime_get()->__environ);
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
		printf(" %s\n", p->content);
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
