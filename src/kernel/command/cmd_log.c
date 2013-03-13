/*
 * kernel/command/cmd_log.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_LOG) && (CONFIG_COMMAND_LOG > 0)

static int do_log(int argc, char ** argv)
{
	struct log_list * list;
	bool_t level_d = FALSE;
	bool_t level_i = FALSE;
	bool_t level_w = FALSE;
	bool_t level_e = FALSE;
	s32_t i, num;

	list = get_log_list();
	if(!list)
		return -1;

	if(argc == 1)
	{
		level_d = TRUE;
		level_i = TRUE;
		level_w = TRUE;
		level_e = TRUE;
	}
	else
	{
		for(i=1; i<argc; i++)
		{
			if( !strcmp((const char *)argv[i], "-d") )
				level_d = TRUE;
			else if( !strcmp((const char *)argv[i], "-i") )
				level_i = TRUE;
			else if( !strcmp((const char *)argv[i], "-w") )
				level_w = TRUE;
			else if( !strcmp((const char *)argv[i], "-e") )
				level_e = TRUE;
			else
			{
				printk("log: invalid option '%s'\r\n", argv[i]);
				printk("usage:\r\n    log [-d|-i|-w|-e]\r\n");
				printk("try 'help log' for more information.\r\n");

				return -1;
			}
		}
	}

	num = log_numberof();
	for(i = 0; i < num; i++)
	{
		list = list_entry((&list->entry)->next, struct log_list, entry);

		switch(list->level)
		{
		case LOG_DEBUG:
			if(level_d == TRUE)
				printk("[D] at line %ld in %s: %s\r\n", list->line, list->file, list->message);
			break;

		case LOG_INFO:
			if(level_i == TRUE)
				printk("[I] at line %ld in %s: %s\r\n", list->line, list->file, list->message);
			break;

		case LOG_WARNNING:
			if(level_w == TRUE)
				printk("[W] at line %ld in %s: %s\r\n", list->line, list->file, list->message);
			break;

		case LOG_ERROR:
			if(level_e == TRUE)
				printk("[E] at line %ld in %s: %s\r\n", list->line, list->file, list->message);
			break;

		default:
			break;
		}
	}

	return 0;
}

static struct command log_cmd = {
	.name		= "log",
	.func		= do_log,
	.desc		= "log of system message with level\r\n",
	.usage		= "log [-d|-i|-w|-e]\r\n",
	.help		= "    print log of system message with level\r\n"
				  "    no arguments for print all level messages\r\n"
				  "    -d    print debug level messages.\r\n"
				  "    -i    print information level messages.\r\n"
				  "    -w    print warnning level messages.\r\n"
				  "    -e    print error level messages.\r\n"
};

static __init void log_cmd_init(void)
{
	if(!command_register(&log_cmd))
		LOG_E("register 'log' command fail");
}

static __exit void log_cmd_exit(void)
{
	if(!command_unregister(&log_cmd))
		LOG_E("unregister 'log' command fail");
}

command_initcall(log_cmd_init);
command_exitcall(log_cmd_exit);

#endif
