/*
 * kernel/command/cmd-tinymembench.c
 */

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    tinymembench [args ...]\r\n");
}

static int do_tinymembench(int argc, char ** argv)
{
	tiny_membench_main();
	return 0;
}

static struct command_t cmd_tinymembench = {
	.name	= "tinymembench",
	.desc	= "This is a simple memory benchmark program",
	.usage	= usage,
	.exec	= do_tinymembench,
};

static __init void tinymembench_cmd_init(void)
{
	register_command(&cmd_tinymembench);
}

static __exit void tinymembench_cmd_exit(void)
{
	unregister_command(&cmd_tinymembench);
}

command_initcall(tinymembench_cmd_init);
command_exitcall(tinymembench_cmd_exit);
