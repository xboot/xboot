/*
 * kernel/command/cmd-sleep.c
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

static void usage(void)
{
	printf("usage:\r\n");
	printf("    sleep [millisecond]\r\n");
}

static int sleep(int argc, char ** argv)
{
	s32_t ms = 1000;

	if(argc == 2)
	{
		ms = strtoul((const char *)argv[1], NULL, 0);
	}

	do {
	    if( ms < 10 )
	    {
	    	mdelay(ms);
	    	ms = 0;
	    }
	    else
	    {
	    	mdelay(10);
	    	ms -= 10;

	    	if(ctrlc())
	    		return -1;
	    }
	}while(ms > 0);

	return 0;
}

static struct command_t cmd_sleep = {
	.name	= "sleep",
	.desc	= "pause cpu for a specified time",
	.usage	= usage,
	.exec	= sleep,
};

static __init void sleep_cmd_init(void)
{
	command_register(&cmd_sleep);
}

static __exit void sleep_cmd_exit(void)
{
	command_unregister(&cmd_sleep);
}

command_initcall(sleep_cmd_init);
command_exitcall(sleep_cmd_exit);
