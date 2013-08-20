/*
 * arch/arm/lib/cpu/cmd_mmuinfo.c
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
#include <command/command.h>

#if	defined(CONFIG_COMMAND_MMUINFO) && (CONFIG_COMMAND_MMUINFO > 0)

static char * inner_attr[] = {
	"0b000 Non-cacheable",
	"0b001 Strongly-ordered",
	"0b010 (reserved)",
	"0b011 Device",
	"0b100 (reserved)",
	"0b101 Write-Back, Write-Allocate",
	"0b110 Write-Through",
	"0b111 Write-Back, no Write-Allocate",
};

static char * outer_attr[] = {
	"0b00 Non-cacheable",
	"0b01 Write-Back, Write-Allocate",
	"0b10 Write-Through, no Write-Allocate",
	"0b11 Write-Back, no Write-Allocate",
};

static void decode_par(unsigned long par)
{
	printk("  Physical Address [31:12]: 0x%08lx\r\n", par & 0xFFFFF000);
	printk("  Reserved [11]:            0x%lx\r\n", (par >> 11) & 0x1);
	printk("  Not Outer Shareable [10]: 0x%lx\r\n", (par >> 10) & 0x1);
	printk("  Non-Secure [9]:           0x%lx\r\n", (par >> 9) & 0x1);
	printk("  Impl. def. [8]:           0x%lx\r\n", (par >> 8) & 0x1);
	printk("  Shareable [7]:            0x%lx\r\n", (par >> 7) & 0x1);
	printk("  Inner mem. attr. [6:4]:   0x%lx (%s)\r\n", (par >> 4) & 0x7, inner_attr[(par >> 4) & 0x7]);
	printk("  Outer mem. attr. [3:2]:   0x%lx (%s)\r\n", (par >> 2) & 0x3, outer_attr[(par >> 2) & 0x3]);
	printk("  SuperSection [1]:         0x%lx\r\n", (par >> 1) & 0x1);
	printk("  Failure [0]:              0x%lx\r\n", (par >> 0) & 0x1);
}

static int mmuinfo(int argc, char ** argv)
{
	unsigned long addr = 0, priv_read, priv_write;

	if(argc < 2)
	{
		printk("usage:\r\n    mmuinfo <ADDRESS>\r\n");
		return -1;
	}

	addr = strtoul(argv[1], NULL, 0);

	__asm__ __volatile__(
		"mcr    p15, 0, %0, c7, c8, 0   @ write VA to PA translation (priv read)\n"
		:
		: "r" (addr)
		: "memory");

	__asm__ __volatile__(
		"mrc    p15, 0, %0, c7, c4, 0   @ read PAR\n"
		: "=r" (priv_read)
		:
		: "memory");

	__asm__ __volatile__(
		"mcr    p15, 0, %0, c7, c8, 1   @ write VA to PA translation (priv write)\n"
		:
		: "r" (addr)
		: "memory");

	__asm__ __volatile__(
		"mrc    p15, 0, %0, c7, c4, 0   @ read PAR\n"
		: "=r" (priv_write)
		:
		: "memory");

	printk("PAR result for 0x%08lx: \r\n", addr);
	printk(" privileged read: 0x%08lx\r\n", priv_read);
	decode_par(priv_read);
	printk(" privileged write: 0x%08lx\r\n", priv_write);
	decode_par(priv_write);

	return 0;
}

static struct command_t mmuinfo_cmd = {
	.name		= "mmuinfo",
	.func		= mmuinfo,
	.desc		= "show mmu/cache information\r\n",
	.usage		= "mmuinfo <ADDRESS>\r\n",
	.help		= "    show mmu/cache information for an address\r\n"
};

static __init void mmuinfo_cmd_init(void)
{
	if(command_register(&mmuinfo_cmd))
		LOG("Register command 'mmuinfo'");
	else
		LOG("Failed to register command 'mmuinfo'");
}

static __exit void mmuinfo_cmd_exit(void)
{
	if(command_unregister(&mmuinfo_cmd))
		LOG("Unegister command 'mmuinfo'");
	else
		LOG("Failed to unregister command 'mmuinfo'");
}

core_initcall(mmuinfo_cmd_init);
core_exitcall(mmuinfo_cmd_exit);

#endif
