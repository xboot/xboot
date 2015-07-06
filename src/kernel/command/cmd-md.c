/*
 * kernel/command/cmd-md.c
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

#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("Usage:\r\n");
	printf("    md [-b|-w|-l] address [-c count]\r\n");
}

static int do_md(int argc, char ** argv)
{
	s32_t base_addr = 0, nbytes = 64;
	s32_t i, size = 1;
	u8_t linebuf[16], line_len;

	if(argc < 2)
	{
		usage();
		return -1;
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
			usage();
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

		printf("%08lx: ", base_addr);
		if(size == 1)
		{
			for(i=0; i<line_len; i+= size)
				*((u8_t *)(&linebuf[i])) = *((u8_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printf(" %02lx", *((u8_t *)(&linebuf[i])));
		}
		else if(size == 2)
		{
			for(i=0; i<line_len; i+= size)
				*((u16_t *)(&linebuf[i])) = *((u16_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printf(" %04lx", *((u16_t *)(&linebuf[i])));
		}
		else if(size == 4)
		{
			for(i=0; i<line_len; i+= size)
				*((u32_t *)(&linebuf[i])) = *((u32_t *)(base_addr+i));

			for(i=0; i<line_len; i+= size)
				printf(" %08lx", *((u32_t *)(&linebuf[i])));
		}

		printf("%*s", (16-line_len)*2+(16-line_len)/size+4, (s8_t*)"");
		for(i=0; i<line_len; i++)
		{
			if( (linebuf[i] < 0x20) || (linebuf[i] > 0x7e) )
				printf(".");
			else
				printf("%c", linebuf[i]);
		}

		base_addr += line_len;
		nbytes -= line_len;
		printf("\r\n");

		if(ctrlc())
			return -1;
	}

	return 0;
}

static struct command_t cmd_md = {
	.name	= "md",
	.desc	= "memory display\r\n",
	.usage	= usage,
	.exec	= do_md,
};

static __init void md_cmd_init(void)
{
	command_register(&cmd_md);
}

static __exit void md_cmd_exit(void)
{
	command_unregister(&cmd_md);
}

command_initcall(md_cmd_init);
command_exitcall(md_cmd_exit);
