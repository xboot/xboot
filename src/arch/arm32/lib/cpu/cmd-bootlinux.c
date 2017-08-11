/*
 * cmd-bootlinux.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    bootlinux <kernel address> <dtb address>\r\n");
}

static int do_bootlinux(int argc, char ** argv)
{
	struct machine_t * mach = get_machine();
	void * kernel;
	void * dtb;

	if(!mach || (argc != 3))
	{
		usage();
		return -1;
	}

	kernel = (void (*)(void *, void *, void *, void *))strtoul(argv[1], NULL, 0);
	dtb = (void *)strtoul(argv[2], NULL, 0);

	/* Now, booting linux */
	printf("Kernel address: 0x%08lx, dtb address: 0x%08lx\r\n", (uint32_t)kernel, (uint32_t)dtb);
	printf("Now, booting linux ......\r\n");

	machine_cleanup();
	((void (*)(void * dtb, void * res0, void * res1, void * res2))kernel)(dtb, 0, 0, 0);

	return 0;
}

static struct command_t cmd_bootlinux = {
	.name	= "bootlinux",
	.desc	= "boot aarch32 linux kernel",
	.usage	= usage,
	.exec	= do_bootlinux,
};

static __init void bootlinux_cmd_init(void)
{
	register_command(&cmd_bootlinux);
}

static __exit void bootlinux_cmd_exit(void)
{
	unregister_command(&cmd_bootlinux);
}

command_initcall(bootlinux_cmd_init);
command_exitcall(bootlinux_cmd_exit);
