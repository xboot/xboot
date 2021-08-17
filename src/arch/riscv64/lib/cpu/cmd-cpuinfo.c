/*
 * cmd-cpuinfo.c
 */

#include <xboot.h>
#include <riscv64.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    cpuinfo\r\n");
}

static int do_cpuinfo(int argc, char ** argv)
{
	uint64_t v;

	v = csr_read(mvendorid);
	printf("mvendorid:  0x%016lx\r\n", v);
	v = csr_read(marchid);
	printf("marchid:    0x%016lx\r\n", v);
	v = csr_read(mimpid);
	printf("mimpid:     0x%016lx\r\n", v);

	v = csr_read(mstatus);
	printf("mstatus:    0x%016lx\r\n", v);
	v = csr_read(misa);
	printf("misa:       0x%016lx\r\n", v);
	v = csr_read(medeleg);
	printf("medeleg:    0x%016lx\r\n", v);
	v = csr_read(mideleg);
	printf("mideleg:    0x%016lx\r\n", v);
	v = csr_read(mie);
	printf("mie:        0x%016lx\r\n", v);
	v = csr_read(mtvec);
	printf("mtvec:      0x%016lx\r\n", v);
	v = csr_read(mcounteren);
	printf("mcounteren: 0x%016lx\r\n", v);

	v = csr_read(mscratch);
	printf("mscratch:   0x%016lx\r\n", v);
	v = csr_read(mepc);
	printf("mepc:       0x%016lx\r\n", v);
	v = csr_read(mcause);
	printf("mcause:     0x%016lx\r\n", v);
	v = csr_read(mtval);
	printf("mtval:      0x%016lx\r\n", v);
	v = csr_read(mip);
	printf("mip:        0x%016lx\r\n", v);

	return 0;
}

static struct command_t cmd_cpuinfo = {
	.name	= "cpuinfo",
	.desc	= "show information about CPU",
	.usage	= usage,
	.exec	= do_cpuinfo,
};

static __init void cpuinfo_cmd_init(void)
{
	register_command(&cmd_cpuinfo);
}

static __exit void cpuinfo_cmd_exit(void)
{
	unregister_command(&cmd_cpuinfo);
}

command_initcall(cpuinfo_cmd_init);
command_exitcall(cpuinfo_cmd_exit);
