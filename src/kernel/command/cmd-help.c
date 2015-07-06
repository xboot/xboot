/*
 * kernel/command/cmd-help.c
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

#include <command/command.h>

extern struct command_list_t * command_list;

static void usage(void)
{
	printf("Usage:\r\n");
	printf("    help [COMMAND] ...\r\n");
}

static int do_help(int argc, char ** argv)
{
	struct command_list_t * list;
	struct list_head * pos;
	struct command_t ** cmd_array;
	struct command_t * cmd;
	s32_t i = 0, j, k = 0, cmd_num, swaps;

	if(argc == 1)
	{
		i = 0;
		cmd_num = command_number();
		cmd_array = malloc(sizeof(struct command_t *) * cmd_num);

		if(!cmd_array)
		{
			printf("malloc command array fail for sort command list\r\n");
			return -1;
		}

		for(pos = (&command_list->entry)->next; pos != (&command_list->entry); pos = pos->next)
		{
			list = list_entry(pos, struct command_list_t, entry);
			cmd_array[i++] = list->cmd;
			j = strlen(list->cmd->name);
			if(j > k)	k = j;
		}

		for(i = cmd_num - 1; i > 0; --i)
		{
			swaps = 0;
			for(j=0; j<i; ++j)
			{
				if (strcmp(cmd_array[j]->name, cmd_array[j + 1]->name) > 0)
				{
					cmd = cmd_array[j];
					cmd_array[j] = cmd_array[j + 1];
					cmd_array[j + 1] = cmd;
					++swaps;
				}
			}
			if(!swaps)
				break;
		}

		for(i = 0; i < cmd_num; i++)
		{
			printf(" %s%*s - %s\r\n",cmd_array[i]->name, k-strlen(cmd_array[i]->name), "", cmd_array[i]->desc);
		}
		free(cmd_array);
	}
	else
	{
		for (i = 1; i < argc; i++)
		{
			if( (cmd = command_search((char*)argv[i])) != NULL )
			{
				printf("%s - %s\r\n", cmd->name, cmd->desc);
				cmd->usage();
			}
			else
			{
				printf("unknown command '%s' - try 'help' without arguments for list of all known commands\r\n", argv[i]);
			}
		}
	}

	return 0;
}

static struct command_t cmd_help = {
	.name	= "help",
	.desc	= "show usage information for command",
	.usage	= usage,
	.exec	= do_help,
};

static __init void help_cmd_init(void)
{
	command_register(&cmd_help);
}

static __exit void help_cmd_exit(void)
{
	command_unregister(&cmd_help);
}

command_initcall(help_cmd_init);
command_exitcall(help_cmd_exit);
