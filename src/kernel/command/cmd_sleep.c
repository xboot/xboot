/*
 * kernel/command/cmd_sleep.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <version.h>
#include <vsprintf.h>
#include <time/delay.h>
#include <shell/ctrlc.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <shell/command.h>


#if	defined(CONFIG_COMMAND_SLEEP) && (CONFIG_COMMAND_SLEEP > 0)

static x_s32 sleep(x_s32 argc, const x_s8 **argv)
{
	x_s32 ms = 1000;

	if(argc == 2)
	{
		ms = simple_strtou32(argv[1], NULL, 0);
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

static struct command sleep_cmd = {
	.name		= "sleep",
	.func		= sleep,
	.desc		= "pause cpu for a specified time\r\n",
	.usage		= "sleep [millisecond]\r\n",
	.help		= "    delay for a specified amount of time\r\n"
				  "    default pause cpu for 1000 millisecond.\r\n"
};

static __init void sleep_cmd_init(void)
{
	if(!command_register(&sleep_cmd))
		LOG_E("register 'sleep' command fail");
}

static __exit void sleep_cmd_exit(void)
{
	if(!command_unregister(&sleep_cmd))
		LOG_E("unregister 'sleep' command fail");
}

module_init(sleep_cmd_init, LEVEL_COMMAND);
module_exit(sleep_cmd_exit, LEVEL_COMMAND);

#endif
