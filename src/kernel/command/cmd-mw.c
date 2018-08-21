/*
 * kernel/command/cmd-mw.c
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

#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mw [-b|-w|-l] address value [-c count]\r\n");
}

static int do_mw(int argc, char ** argv)
{
	u32_t base_addr = 0, value = 0, c = 1;
	u32_t index = 0, i, size = 1;

	if(argc < 3)
	{
		usage();
		return -1;
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
				printf("mw: the parmater of write count is zero by '-c %s'", argv[i+1]);
				return -1;
			}
			i++;
		}
		else if(*argv[i] == '-')
		{
			usage();
			return -1;
		}
		else if(*argv[i] != '-' && strcmp((const char *)argv[i], "-") != 0)
		{
			if(index == 0)
				base_addr = strtoul((const char *)argv[i], NULL, 0);
			else if(index == 1)
				value = strtoul((const char *)argv[i], NULL, 0);
			else if(index >= 2)
			{
				printf("mw: invalid paramter '%s'\r\n", argv[i]);
				printf("try 'help mw' for more information.\r\n");
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
			printf("warnning: the address has been fixed to 0x%08lx.\r\n", base_addr);
		}
	}
	else if(size == 4)
	{
		if(base_addr & 0x00000003)
		{
			base_addr = base_addr & (~0x00000003);
			printf("warnning: the address has been fixed to 0x%08lx.\r\n", base_addr);
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
	printf("write done.\r\n");

	return 0;
}

static struct command_t cmd_mw = {
	.name	= "mw",
	.desc	= "memory write",
	.usage	= usage,
	.exec	= do_mw,
};

static __init void mw_cmd_init(void)
{
	register_command(&cmd_mw);
}

static __exit void mw_cmd_exit(void)
{
	unregister_command(&cmd_mw);
}

command_initcall(mw_cmd_init);
command_exitcall(mw_cmd_exit);
