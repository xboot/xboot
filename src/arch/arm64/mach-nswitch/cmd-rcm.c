/*
 * cmd-rcm.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    rcm\r\n");
}

static int do_rcm(int argc, char ** argv)
{
	virtual_addr_t virt = phys_to_virt(0x7000e400);
	write32(virt + 0x50, (read32(virt + 0x50) & ~(1 << 1)) | (1 << 1));
	write32(virt + 0x00, (read32(virt + 0x00) & ~(1 << 4)) | (1 << 4));
	return 0;
}

static struct command_t cmd_rcm = {
	.name	= "rcm",
	.desc	= "reboot and enter to usb recovery mode",
	.usage	= usage,
	.exec	= do_rcm,
};

static __init void rcm_cmd_init(void)
{
	register_command(&cmd_rcm);
}

static __exit void rcm_cmd_exit(void)
{
	unregister_command(&cmd_rcm);
}

command_initcall(rcm_cmd_init);
command_exitcall(rcm_cmd_exit);
