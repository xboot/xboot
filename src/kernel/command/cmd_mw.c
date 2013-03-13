/*
 * kernel/command/cmd_mw.c
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
#include <stdlib.h>
#include <version.h>
#include <shell/ctrlc.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_MW) && (CONFIG_COMMAND_MW > 0)

static int mw(int argc, char ** argv)
{
	u32_t base_addr = 0, value = 0, c = 1;
	u32_t index = 0, i, size = 1;

	if(argc < 3)
	{
		printk("usage:\r\n    mw [-b|-w|-l] address value [-c count]\r\n");
		return (-1);
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i], "-b") )
			size = 1;
		else if( !strcmp((const char *)argv[i], "-w") )
			size = 2;
		else if( !strcmp((const char *)argv[i], "-l") )
			size = 4;
		else if( !strcmp((const char *)argv[i], "-c") && (argc > i+1))
		{
			c = strtoul((const char *)argv[i+1], NULL, 0);
			if(c == 0)
			{
				printk("mw: the parmater of write count is zero by '-c %s'", argv[i+1]);
				return -1;
			}
			i++;
		}
		else if(*argv[i] == '-')
		{
			printk("mw: invalid option '%s'\r\n", argv[i]);
			printk("usage:\r\n    mw [-b|-w|-l] address value [-c count]\r\n");
			printk("try 'help mw' for more information.\r\n");
			return (-1);
		}
		else if(*argv[i] != '-' && strcmp((const char *)argv[i], "-") != 0)
		{
			if(index == 0)
				base_addr = strtoul((const char *)argv[i], NULL, 0);
			else if(index == 1)
				value = strtoul((const char *)argv[i], NULL, 0);
			else if(index >= 2)
			{
				printk("mw: invalid paramter '%s'\r\n", argv[i]);
				printk("try 'help mw' for more information.\r\n");
				return (-1);
			}
			index++;
		}
	}

	if(size == 2)
	{
		if(base_addr & 0x00000001)
		{
			base_addr = base_addr & (~0x00000001);
			printk("warnning: the address has been fixed to 0x%08lx.\r\n", base_addr);
		}
	}
	else if(size == 4)
	{
		if(base_addr & 0x00000003)
		{
			base_addr = base_addr & (~0x00000003);
			printk("warnning: the address has been fixed to 0x%08lx.\r\n", base_addr);
		}
	}

	c = c * size;

	for(i = 0; i< c; i+=size)
	{
		if(size == 1)
		{
			*((u8_t *)(base_addr+i)) = value;
		}
		else if(size == 2)
		{
			*((u16_t *)(base_addr+i)) = value;
		}
		else if(size == 4)
		{
			*((u32_t *)(base_addr+i)) = value;
		}

		if(ctrlc())
			return -1;
	}
	printk("write done.\r\n");

	return 0;
}

static struct command mw_cmd = {
	.name		= "mw",
	.func		= mw,
	.desc		= "memory write\r\n",
	.usage		= "mw [-b|-w|-l] address [-n length]\r\n",
	.help		= "    write value to memory at address.\r\n"
				  "    -b    write value with byte format (default)\r\n"
				  "    -w    write value with half word format\r\n"
				  "    -l    write value with word format\r\n"
				  "    -c    the count of write value to memory (default is one)\r\n"
};

static __init void mw_cmd_init(void)
{
	if(!command_register(&mw_cmd))
		LOG_E("register 'mw' command fail");
}

static __exit void mw_cmd_exit(void)
{
	if(!command_unregister(&mw_cmd))
		LOG_E("unregister 'mw' command fail");
}

command_initcall(mw_cmd_init);
command_exitcall(mw_cmd_exit);

#endif
