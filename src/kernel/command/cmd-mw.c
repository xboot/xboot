/*
 * kernel/command/cmd-mw.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	printf("    mw [-b|-w|-l|-q] address value [-c count]\r\n");
}

static int do_mw(int argc, char ** argv)
{
	virtual_addr_t addr = 0;
	u64_t value = 0;
	int size = 1, n = 1;
	int index = 0;
	int i;

	if(argc < 3)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-b"))
			size = 1;
		else if(!strcmp(argv[i], "-w"))
			size = 2;
		else if(!strcmp(argv[i], "-l"))
			size = 4;
		else if(!strcmp(argv[i], "-q"))
			size = 8;
		else if(!strcmp(argv[i], "-c") && (argc > i + 1))
		{
			n = strtoul(argv[i + 1], NULL, 0);
			if(n == 0)
			{
				printf("mw: the writing count is zero by '-c %s'", argv[i + 1]);
				return -1;
			}
			i++;
		}
		else if(*argv[i] == '-')
		{
			usage();
			return -1;
		}
		else if(*argv[i] != '-' && strcmp(argv[i], "-") != 0)
		{
			if(index == 0)
				addr = phys_to_virt(strtoul(argv[i], NULL, 0));
			else if(index == 1)
				value = strtoull(argv[i], NULL, 0);
			else if(index >= 2)
			{
				printf("mw: invalid paramter '%s'\r\n", argv[i]);
				printf("try 'help mw' for more information.\r\n");
				return (-1);
			}
			index++;
		}
	}
	if(size == 1)
	{
		addr &= ~((virtual_addr_t)0x0);
	}
	else if(size == 2)
	{
		addr &= ~((virtual_addr_t)0x1);
	}
	else if(size == 4)
	{
		addr &= ~((virtual_addr_t)0x3);
	}
	else if(size == 8)
	{
		addr &= ~((virtual_addr_t)0x7);
	}
	n = n * size;

	for(i = 0; i < n; i += size)
	{
		if(size == 1)
			write8((virtual_addr_t)(addr + i), (u8_t)value);
		else if(size == 2)
			write16((virtual_addr_t)(addr + i), (u16_t)value);
		else if(size == 4)
			write32((virtual_addr_t)(addr + i), (u32_t)value);
		else if(size == 8)
			write64((virtual_addr_t)(addr + i), (u64_t)value);
		if(ctrlc())
			return -1;
	}
	printf("write done.\r\n");

	return 0;
}

static struct command_t cmd_mw = {
	.name	= "mw",
	.desc	= "write values to memory region",
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
