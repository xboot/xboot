/*
 * cmd-mmuinfo.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mmuinfo <ADDRESS>\r\n");
}

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
	printf("    Physical Address [31:12]: 0x%08lx\r\n", par & 0xFFFFF000);
	printf("    Reserved [11]:            0x%lx\r\n", (par >> 11) & 0x1);
	printf("    Not Outer Shareable [10]: 0x%lx\r\n", (par >> 10) & 0x1);
	printf("    Non-Secure [9]:           0x%lx\r\n", (par >> 9) & 0x1);
	printf("    Impl. def. [8]:           0x%lx\r\n", (par >> 8) & 0x1);
	printf("    Shareable [7]:            0x%lx\r\n", (par >> 7) & 0x1);
	printf("    Inner mem. attr. [6:4]:   0x%lx (%s)\r\n", (par >> 4) & 0x7, inner_attr[(par >> 4) & 0x7]);
	printf("    Outer mem. attr. [3:2]:   0x%lx (%s)\r\n", (par >> 2) & 0x3, outer_attr[(par >> 2) & 0x3]);
	printf("    SuperSection [1]:         0x%lx\r\n", (par >> 1) & 0x1);
	printf("    Failure [0]:              0x%lx\r\n", (par >> 0) & 0x1);
}

static int do_mmuinfo(int argc, char ** argv)
{
	unsigned long addr = 0, priv_read, priv_write;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	addr = strtoul(argv[1], NULL, 0);

	__asm__ __volatile__(
		"mcr p15, 0, %0, c7, c8, 0\n"
		:
		: "r" (addr)
		: "memory");

	__asm__ __volatile__(
		"mrc p15, 0, %0, c7, c4, 0\n"
		: "=r" (priv_read)
		:
		: "memory");

	__asm__ __volatile__(
		"mcr p15, 0, %0, c7, c8, 1\n"
		:
		: "r" (addr)
		: "memory");

	__asm__ __volatile__(
		"mrc p15, 0, %0, c7, c4, 0\n"
		: "=r" (priv_write)
		:
		: "memory");

	printf("PAR result for 0x%08lx: \r\n", addr);
	printf("  privileged read: 0x%08lx\r\n", priv_read);
	decode_par(priv_read);
	printf("  privileged write: 0x%08lx\r\n", priv_write);
	decode_par(priv_write);

	return 0;
}

static struct command_t cmd_mmuinfo = {
	.name	= "mmuinfo",
	.desc	= "show mmu/cache information of an address",
	.usage	= usage,
	.exec	= do_mmuinfo,
};

static __init void mmuinfo_cmd_init(void)
{
	register_command(&cmd_mmuinfo);
}

static __exit void mmuinfo_cmd_exit(void)
{
	unregister_command(&cmd_mmuinfo);
}

command_initcall(mmuinfo_cmd_init);
command_exitcall(mmuinfo_cmd_exit);
