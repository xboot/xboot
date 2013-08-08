/*
 * arch/arm/lib/cpu/cmd_cpuinfo.c
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

#if	defined(CONFIG_COMMAND_CPUINFO) && (CONFIG_COMMAND_CPUINFO > 0)

enum {
	CPU_ARCH_UNKNOWN	= 0,
	CPU_ARCH_ARMv3		= 1,
	CPU_ARCH_ARMv4		= 2,
	CPU_ARCH_ARMv4T		= 3,
	CPU_ARCH_ARMv5		= 4,
	CPU_ARCH_ARMv5T		= 5,
	CPU_ARCH_ARMv5TE	= 6,
	CPU_ARCH_ARMv5TEJ	= 7,
	CPU_ARCH_ARMv6		= 8,
	CPU_ARCH_ARMv7		= 9,
};

static void decode_cache(unsigned long size)
{
	int linelen = 1 << ((size & 0x3) + 3);
	int mult = 2 + ((size >> 2) & 1);
	int cache_size = mult << (((size >> 6) & 0x7) + 8);

	if (((size >> 2) & 0xf) == 1)
		printk("no cache\r\n");
	else
		printk("%d bytes (linelen = %d)\r\n", cache_size, linelen);
}

static char * crbits[] = {
	"M", "A", "C", "W", "P", "D", "L", "B", "S", "R",
	"F", "Z", "I", "V", "RR", "L4", "DT", "", "IT", "ST", "", "FI", "U", "XP",
	"VE", "EE", "L2", "", "TRE", "AFE", "TE"
};

static int cpuinfo(int argc, char ** argv)
{
	unsigned long mainid, cache, cr;
	char * architecture, * implementer;
	int i;
	int cpu_arch;

	__asm__ __volatile__(
		"mrc    p15, 0, %0, c0, c0, 0   @ read control reg\n"
		: "=r" (mainid)
		:
		: "memory");

	__asm__ __volatile__(
		"mrc    p15, 0, %0, c0, c0, 1   @ read control reg\n"
		: "=r" (cache)
		:
		: "memory");

	__asm__ __volatile__(
		"mrc    p15, 0, %0, c1, c0, 0   @ read control reg\n"
			: "=r" (cr)
			:
			: "memory");

	switch (mainid >> 24)
	{
	case 0x41:
		implementer = "ARM";
		break;
	case 0x44:
		implementer = "Digital Equipment Corp.";
		break;
	case 0x40:
		implementer = "Motorola - Freescale Semiconductor Inc.";
		break;
	case 0x56:
		implementer = "Marvell Semiconductor Inc.";
		break;
	case 0x69:
		implementer = "Intel Corp.";
		break;
	default:
		implementer = "Unknown";
		break;
	}

	if ((mainid & 0x0008f000) == 0)
	{
		cpu_arch = CPU_ARCH_UNKNOWN;
	}
	else if ((mainid & 0x0008f000) == 0x00007000)
	{
		cpu_arch = (mainid & (1 << 23)) ? CPU_ARCH_ARMv4T : CPU_ARCH_ARMv3;
	}
	else if ((mainid & 0x00080000) == 0x00000000)
	{
		cpu_arch = (mainid >> 16) & 7;
		if (cpu_arch)
			cpu_arch += CPU_ARCH_ARMv3;
	}
	else if ((mainid & 0x000f0000) == 0x000f0000)
	{
		unsigned int mmfr0;

		__asm__ __volatile__("mrc	p15, 0, %0, c0, c1, 4" : "=r" (mmfr0));

		if ((mmfr0 & 0x0000000f) >= 0x00000003 || (mmfr0 & 0x000000f0) >= 0x00000030)
			cpu_arch = CPU_ARCH_ARMv7;
		else if ((mmfr0 & 0x0000000f) == 0x00000002 || (mmfr0 & 0x000000f0) == 0x00000020)
			cpu_arch = CPU_ARCH_ARMv6;
		else
			cpu_arch = CPU_ARCH_UNKNOWN;
	}
	else
		cpu_arch = CPU_ARCH_UNKNOWN;

	switch (cpu_arch)
	{
	case CPU_ARCH_ARMv3:
		architecture = "v3";
		break;
	case CPU_ARCH_ARMv4:
		architecture = "v4";
		break;
	case CPU_ARCH_ARMv4T:
		architecture = "v4T";
		break;
	case CPU_ARCH_ARMv5:
		architecture = "v5";
		break;
	case CPU_ARCH_ARMv5T:
		architecture = "v5T";
		break;
	case CPU_ARCH_ARMv5TE:
		architecture = "v5TE";
		break;
	case CPU_ARCH_ARMv5TEJ:
		architecture = "v5TEJ";
		break;
	case CPU_ARCH_ARMv6:
		architecture = "v6";
		break;
	case CPU_ARCH_ARMv7:
		architecture = "v7";
		break;
	case CPU_ARCH_UNKNOWN:
	default:
		architecture = "Unknown";
		break;
	}

	printk("implementer: %s\narchitecture: %s\r\n", implementer, architecture);

	if (cache & (1 << 24))
	{
		/* separate I/D cache */
		printk("I-cache: ");
		decode_cache(cache & 0xfff);
		printk("D-cache: ");
		decode_cache((cache >> 12) & 0xfff);
	}
	else
	{
		/* unified I/D cache */
		printk("cache: ");
		decode_cache(cache & 0xfff);
	}

	printk("Control register: ");
	for (i = 0; i < ARRAY_SIZE(crbits); i++)
		if (cr & (1 << i))
			printk("%s ", crbits[i]);
	printk("\r\n");

	return 0;
}

static struct command_t cpuinfo_cmd = {
	.name		= "cpuinfo",
	.func		= cpuinfo,
	.desc		= "show info about CPU\r\n",
	.usage		= "cpuinfo\r\n",
	.help		= "    show info about CPU\r\n"
};

static __init void cpuinfo_cmd_init(void)
{
	if(command_register(&cpuinfo_cmd))
		LOG("Register command 'cpuinfo'");
	else
		LOG("Fail to register command 'cpuinfo'");
}

static __exit void cpuinfo_cmd_exit(void)
{
	if(command_unregister(&cpuinfo_cmd))
		LOG("Unegister command 'cpuinfo'");
	else
		LOG("Fail to unregister command 'cpuinfo'");
}

command_initcall(cpuinfo_cmd_init);
command_exitcall(cpuinfo_cmd_exit);

#endif
