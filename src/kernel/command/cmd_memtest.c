/*
 * kernel/command/cmd_memtest.c
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
#include <time/delay.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <shell/ctrlc.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_MEMTEST) && (CONFIG_COMMAND_MEMTEST > 0)

static int memtest(int argc, char ** argv)
{
	u32_t base, size;
	u32_t * start, * end;
	u32_t walker, readback, *i;
	s32_t errcnt = 0;

	if(argc == 1)
	{
		base = 0;
		base = base & (~0x00000003);
		size = 0;
		size = size & (~0x00000003);
	}
	else if(argc == 3)
	{
		base = strtoul((const char *)argv[1], NULL, 0);
		base = base & (~0x00000003);
		size = strtoul((const char *)argv[2], NULL, 0);
		size = size & (~0x00000003);
	}
	else
	{
		printk("usage:\r\n    memtest <address> <size>\r\n");
		return (-1);
	}

	printk("testing 0x%08lx .. 0x%08lx\r\n", base, base + size);
	start = (u32_t *)base;

	if(size == 0)
	{
		printk("not do any testing\r\n");
		return 0;
	}

	/* walker one test */
	walker = 0x1;
	end = start + 32;
	for(i = start; i < end; i++)
	{
		*i = walker;
		walker <<= 1;
	}

	walker = 0x1;
	for(i = start; i < end; i++)
	{
		readback = *i;
		if(readback != walker)
		{
			errcnt++;
			printk("error at 0x%08lx: read 0x%lx expected x%lx\r\n", (u32_t)i, readback, walker);
			break;
		}
		walker <<= 1;
	}

	/* address in address test */
	end = (u32_t *)((u32_t)start + size);
	for(i = start; i < end; i++)
	{
		if(((u32_t)i & 0x3ffff) == 0)
		{
			if(ctrlc())
				return -1;
		}

		*i = (u32_t)i;
	}

	for(i = start; i < end; i++)
	{
		if(((u32_t)i & 0x3ffff) == 0)
		{
			if(ctrlc())
				return -1;
		}

		readback = *i;
		if(readback != (u32_t)i)
		{
			errcnt++;
			printk("error at 0x%08lx: read 0x%08lx expected 0x%08lx\r\n", (u32_t)i, readback, (u32_t)i);
			break;
		}
	}

	if(errcnt > 0)
		printk("found %d errors\r\n", errcnt);
	else
		printk("no found any errors\r\n");

	return 0;
}

static struct command memtest_cmd = {
	.name		= "memtest",
	.func		= memtest,
	.desc		= "system memory testing\r\n",
	.usage		= "memtest <address> <size>\r\n",
	.help		= "    checking system memory\r\n"
				  "    default for auto checking memory\r\n"
};

static __init void memtest_cmd_init(void)
{
	if(!command_register(&memtest_cmd))
		LOG_E("register 'memtest' command fail");
}

static __exit void memtest_cmd_exit(void)
{
	if(!command_unregister(&memtest_cmd))
		LOG_E("unregister 'memtest' command fail");
}

command_initcall(memtest_cmd_init);
command_exitcall(memtest_cmd_exit);

#endif
