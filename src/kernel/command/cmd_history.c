/*
 * kernel/command/cmd_history.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <shell/command.h>
#include <shell/history.h>


#if	defined(CONFIG_COMMAND_HISTORY) && (CONFIG_COMMAND_HISTORY > 0)

static x_s32 history(x_s32 argc, const x_s8 **argv)
{
	x_s32 i, n, N, num;
	struct history_list * list;

	n = num = history_numberof();

	for(i=1; i<argc; i++)
	{
		if( !strcmp(argv[i],(x_s8*)"-c") )
		{
			n = num;
			while(n-- > 0)
			{
				history_remove();
			}
			return 0;
		}
		else if( !strcmp(argv[i],(x_s8*)"-a") && (argc > i+1))
		{
			history_add(argv[i+1]);
			i++;
			return 0;
		}
		else if( !strcmp(argv[i],(x_s8*)"-r"))
		{
			history_remove();
			return 0;
		}
		else if(*argv[i] == '-')
		{
			printk("history: invalid option '%s'\r\n", argv[i]);
			printk("usage:\r\n    history [-c|-r|-a CMD] [N]\r\n");
			printk("try 'help history' for more information.\r\n");
			return (-1);
		}
		else if(*argv[i] != '-' && strcmp(argv[i], (x_s8*)"-") != 0)
		{
			n = simple_strtou32(argv[i], NULL, 0);
		}
	}

	N = num < n ? num:n;

	list = get_history_list();

	for(i = 1; i <= num-N; i++)
		list = list_entry((&list->entry)->prev, struct history_list, entry);

	for(; i <= num; i++)
	{
		list = list_entry((&list->entry)->prev, struct history_list, entry);
		printk(" [%2d]  %s\r\n", i, list->cmdline);
	}

	return 0;
}

static struct command history_cmd = {
	.name		= "history",
	.func		= history,
	.desc		= "display the history list\r\n",
	.usage		= "history [-c|-r|-a CMD] [N]\r\n",
	.help		= "    display the history list with line numbers.\r\n"
				  "    -c    the history list will be cleared\r\n"
				  "    -r    deletes the first history entry\r\n"
				  "    -a    append a command to history list\r\n"
				  "     N    list only the last N lines\r\n"
};

static __init void history_cmd_init(void)
{
	if(!command_register(&history_cmd))
		DEBUG_E("register 'history' command fail");
}

static __exit void history_cmd_exit(void)
{
	if(!command_unregister(&history_cmd))
		DEBUG_E("unregister 'history' command fail");
}

module_init(history_cmd_init, LEVEL_COMMAND);
module_exit(history_cmd_exit, LEVEL_COMMAND);

#endif
