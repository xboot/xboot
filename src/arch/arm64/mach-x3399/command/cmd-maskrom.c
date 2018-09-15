/*
 * command/cmd-maskrom.c
 */

#include <rk3399/reg-cru.h>
#include <rk3399/reg-pmu-grf.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    maskrom\r\n");
}

static int do_maskrom(int argc, char ** argv)
{
	write32(phys_to_virt(RK3399_PMU_GRF_BASE) + PMU_GRF_OS_REG0, 0xef08a53c);
	write32(phys_to_virt(RK3399_CRU_BASE) + CRU_GLB_SRST_FST_VALUE, 0xfdb9);
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
