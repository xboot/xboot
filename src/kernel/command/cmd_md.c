/*
 * xboot/kernel/command/cmd_md.c
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
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/ctrlc.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_MD) && (CONFIG_COMMAND_MD > 0)

static int md(int argc, char ** argv)
{
	s32_t base_addr = 0, nbytes = 64;
	s32_t i, size = 1;
	u8_t linebuf[16], line_len;

	if(argc < 2)
	{
		printk("usage:\r\n    md [-b|-w|-l] address [-c count]\r\n");
		return (-1);
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i],"-b") )
			size = 1;
		else if( !strcmp((const char *)argv[i],"-w") )
			size = 2;
		else if( !strcmp((const char *)argv[i],"-l") )
			size = 4;
		else if( !strcmp((const char *)argv[i],"-c") && (argc > i+1))
		{
			nbytes = strtoul((const char *)argv[i+1], NULL, 0);
			i++;
		}
		else if(*argv[i] == '-')
		{
			printk("md: invalid option '%s'\r\n", argv[i]);
			printk("usage:\r\n    md [-b|-w|-l] address [-c count]\r\n");
			printk("try 'help md' for more information.\r\n");
			return (-1);
		}
		else if(*argv[i] != '-' && strcmp((const char *)argv[i], "-") != 0)
		{
			base_addr = strtoul((const char *)argv[i], NULL, 0);
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
				*((u8_t *)(&linebuf[i])) = *((u8_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %02lx", *((u8_t *)(&linebuf[i])));
		}
		else if(size == 2)
		{
			for(i=0; i<line_len; i+= size)
				*((u16_t *)(&linebuf[i])) = *((u16_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %04lx", *((u16_t *)(&linebuf[i])));
		}
		else if(size == 4)
		{
			for(i=0; i<line_len; i+= size)
				*((u32_t *)(&linebuf[i])) = *((u32_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printk(" %08lx", *((u32_t *)(&linebuf[i])));
		}

		printk("%*s", (16-line_len)*2+(16-line_len)/size+4, (s8_t*)"");
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

command_initcall(md_cmd_init);
command_exitcall(md_cmd_exit);

#endif
