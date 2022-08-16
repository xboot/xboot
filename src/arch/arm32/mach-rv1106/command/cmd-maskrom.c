/*
 * command/cmd-maskrom.c
 */

#include <xboot.h>
#include <rv1106/reg-cru.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    maskrom\r\n");
}

static int do_maskrom(int argc, char ** argv)
{
	write32(phys_to_virt(0xff020200), 0xef08a53c);
	write32(phys_to_virt(RV1106_CRU_BASE) + offsetof(struct rv1106_cru_t, glb_srst_fst), 0xfdb9);
	return 0;
}

static struct command_t cmd_maskrom = {
	.name	= "maskrom",
	.desc	= "reboot and enter to maskrom mode",
	.usage	= usage,
	.exec	= do_maskrom,
};

static __init void maskrom_cmd_init(void)
{
	register_command(&cmd_maskrom);
}

static __exit void maskrom_cmd_exit(void)
{
	unregister_command(&cmd_maskrom);
}

command_initcall(maskrom_cmd_init);
command_exitcall(maskrom_cmd_exit);
