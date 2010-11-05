/*
 * xboot/kernel/command/cmd_md.c
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
#include <vsprintf.h>
#include <configs.h>
#include <version.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/ctrlc.h>
#include <shell/command.h>


#if	defined(CONFIG_COMMAND_MD) && (CONFIG_COMMAND_MD > 0)

static x_s32 md(x_s32 argc, const x_s8 **argv)
{
	x_s32 base_addr = 0, nbytes = 64;
	x_s32 i, size = 1;
	x_u8 linebuf[16], line_len;

	if(argc < 2)
	{
		printk("usage:\r\n    md [-b|-w|-l] address [-c count]\r\n");
		return (-1);
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp(argv[i],(x_s8*)"-b") )
			size = 1;
		else if( !strcmp(argv[i],(x_s8*)"-w") )
			size = 2;
		else if( !strcmp(argv[i],(x_s8*)"-l") )
			size = 4;
		else if( !strcmp(argv[i],(x_s8*)"-c") && (argc > i+1))
		{
			nbytes = simple_strtou32(argv[i+1], NULL, 0);
			i++;
		}
		else if(*argv[i] == '-')
		{
			printk("md: invalid option '%s'\r\n", argv[i]);
			printk("usage:\r\n    md [-b|-w|-l] address [-c count]\r\n");
			printk("try 'help md' for more information.\r\n");
			return (-1);
		}
		else if(*argv[i] != '-' && strcmp(argv[i], (x_s8*)"-") != 0)
		{
			base_addr = simple_strtou32(argv[i], NULL, 0);
		}
	}

	if(size == 2)
	{
		base_addr = base_addr & (~0x00000001);
	}
	else if(size == 4)
	{
		base_addr = base_addr & (~0x00000003);
	}

	nbytes = nbytes * size;

	while(nbytes > 0)
	{
		line_len = (nbytes > 16) ? 16:nbytes;

		printk("%08lx: ", base_addr);
		if(size == 1)
		{
			for(i=0; i<line_len; i+= size)
				*((x_u8 *)(&linebuf[i])) = *((x_u8 *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %02lx", *((x_u8 *)(&linebuf[i])));
		}
		else if(size == 2)
		{
			for(i=0; i<line_len; i+= size)
				*((x_u16 *)(&linebuf[i])) = *((x_u16 *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %04lx", *((x_u16 *)(&linebuf[i])));
		}
		else if(size == 4)
		{
			for(i=0; i<line_len; i+= size)
				*((x_u32 *)(&linebuf[i])) = *((x_u32 *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %08lx", *((x_u32 *)(&linebuf[i])));
		}

		printk("%*s", (16-line_len)*2+(16-line_len)/size+4, (x_s8*)"");
		for(i=0; i<line_len; i++)
		{
			if( (linebuf[i] < 0x20) || (linebuf[i] > 0x7e) )
				printk(".");
			else
				printk("%c", linebuf[i]);
		}

		base_addr += line_len;
		nbytes -= line_len;
		printk("\r\n");

		if(ctrlc())
			return -1;
	}

	return 0;
}

static struct command md_cmd = {
	.name		= "md",
	.func		= md,
	.desc		= "memory display\r\n",
	.usage		= "md [-b|-w|-l] address [-n length]\r\n",
	.help		= "    display memory at address.\r\n"
				  "    -b    display format with byte mode (default)\r\n"
				  "    -w    display format with half word mode\r\n"
				  "    -l    display format with word mode\r\n"
				  "    -c    the count of display memory (default is 64 bytes)\r\n"
};

static __init void md_cmd_init(void)
{
	if(!command_register(&md_cmd))
		LOG_E("register 'md' command fail");
}

static __exit void md_cmd_exit(void)
{
	if(!command_unregister(&md_cmd))
		LOG_E("unregister 'md' command fail");
}

module_init(md_cmd_init, LEVEL_COMMAND);
module_exit(md_cmd_exit, LEVEL_COMMAND);

#endif
