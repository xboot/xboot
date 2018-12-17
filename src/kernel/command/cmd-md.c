/*
 * kernel/command/cmd-md.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
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
	.desc	= "memory display",
	.usage	= usage,
	.exec	= do_md,
};

static __init void md_cmd_init(void)
{
	register_command(&cmd_md);
}

static __exit void md_cmd_exit(void)
{
	unregister_command(&cmd_md);
}

command_initcall(md_cmd_init);
command_exitcall(md_cmd_exit);
