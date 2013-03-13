/*
 * kernel/command/cmd_sleep.c
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
#include <string.h>
#include <version.h>
#include <stdlib.h>
#include <time/delay.h>
#include <shell/ctrlc.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_SLEEP) && (CONFIG_COMMAND_SLEEP > 0)

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

command_initcall(sleep_cmd_init);
command_exitcall(sleep_cmd_exit);

#endif
