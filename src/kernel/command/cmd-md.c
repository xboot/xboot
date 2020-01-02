/*
 * kernel/command/cmd-md.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
	printf("    md [-b|-w|-l|-q] address [-c count]\r\n");
}

static int do_md(int argc, char ** argv)
{
	virtual_addr_t addr = 0;
	char buf[16];
	int n = 64, size = 1;
	int i, len;
	u8_t b; u16_t w; u32_t l; u64_t q;

	if(argc < 2)
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
			i++;
		}
		else if(*argv[i] == '-')
		{
			usage();
			return (-1);
		}
		else if(*argv[i] != '-' && strcmp(argv[i], "-") != 0)
		{
			addr = phys_to_virt(strtoul(argv[i], NULL, 0));
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

	while(n > 0)
	{
		len = (n > 16) ? 16 : n;
		printf("%08lx: ", addr);
		if(size == 1)
		{
			for(i = 0; i < len; i += size)
			{
				write8((virtual_addr_t)(&buf[i]), (b = read8(addr + i)));
				printf(" %02lx", b);
			}
		}
		else if(size == 2)
		{
			for(i = 0; i < len; i += size)
			{
				write16((virtual_addr_t)(&buf[i]), (w = read16(addr + i)));
				printf(" %04lx", w);
			}
		}
		else if(size == 4)
		{
			for(i = 0; i < len; i += size)
			{
				write32((virtual_addr_t)(&buf[i]), (l = read32(addr + i)));
				printf(" %08lx", l);
			}
		}
		else if(size == 8)
		{
			for(i = 0; i < len; i += size)
			{
				write64((virtual_addr_t)(&buf[i]), (q = read64(addr + i)));
				printf(" %016llx", q);
			}
		}
		printf("%*s", (16 - len) * 2 + (16 - len) / size + 4, "");
		for(i = 0; i < len; i++)
		{
			if((buf[i] < 0x20) || (buf[i] > 0x7e))
				printf(".");
			else
				printf("%c", buf[i]);
		}
		addr += len;
		n -= len;
		printf("\r\n");

		if(ctrlc())
			return -1;
	}
	return 0;
}

static struct command_t cmd_md = {
	.name	= "md",
	.desc	= "dump a memory region",
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
