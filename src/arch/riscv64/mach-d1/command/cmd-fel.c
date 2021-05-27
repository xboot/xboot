/*
 * command/cmd-fel.c
 */

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    fel\r\n");
}

static int do_fel(int argc, char ** argv)
{
	virtual_addr_t virt = phys_to_virt(0x07090000);

	do {
		write32(virt + 0x100 + 0x8, 0x5aa5a55a);
		mb();
		smp_mb();
	} while(read32(virt + 0x100 + 0x8) != 0x5aa5a55a);
	machine_reboot();

	return 0;
}

static struct command_t cmd_fel = {
	.name	= "fel",
	.desc	= "reboot and enter to fel mode",
	.usage	= usage,
	.exec	= do_fel,
};

static __init void fel_cmd_init(void)
{
	register_command(&cmd_fel);
}

static __exit void fel_cmd_exit(void)
{
	unregister_command(&cmd_fel);
}

command_initcall(fel_cmd_init);
command_exitcall(fel_cmd_exit);
