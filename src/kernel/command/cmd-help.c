/*
 * kernel/command/cmd-help.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
	printf("    help [command ...]\r\n");
}

static int do_help(int argc, char ** argv)
{
	struct command_list_t * list;
	struct list_head * pos;
	struct command_t ** cmds;
	struct command_t * c;
	int i = 0, j = 0, k = 0, n, swaps;

	if(argc == 1)
	{
		i = 0;
		n = total_command_number();
		cmds = malloc(sizeof(struct command_t *) * n);

		if(!cmds)
			return -1;

		for(pos = (&__command_list.entry)->next; pos != (&__command_list.entry); pos = pos->next)
		{
			list = list_entry(pos, struct command_list_t, entry);
			cmds[i++] = list->cmd;
			j = strlen(list->cmd->name);
			if(j > k)
				k = j;
		}

		for(i = n - 1; i > 0; --i)
		{
			swaps = 0;
			for(j = 0; j < i; ++j)
			{
				if (strcmp(cmds[j]->name, cmds[j + 1]->name) > 0)
				{
					c = cmds[j];
					cmds[j] = cmds[j + 1];
					cmds[j + 1] = c;
					++swaps;
				}
			}
			if(!swaps)
				break;
		}

		for(i = 0; i < n; i++)
		{
			printf(" %s%*s - %s\r\n",cmds[i]->name, k - strlen(cmds[i]->name), "", cmds[i]->desc);
		}
		free(cmds);
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
