/*
 * cmd-exit.c
 */

#include <xboot.h>
#include <command/command.h>
#include <sandbox.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    exit\r\n");
}

static int do_exit(int argc, char ** argv)
{
	sandbox_exit();
	return 0;
}

static struct command_t cmd_exit = {
	.name	= "exit",
	.desc	= "terminate the xboot program",
	.usage	= usage,
	.exec	= do_exit,
};

static __init void exit_cmd_init(void)
{
	register_command(&cmd_exit);
}

static __exit void exit_cmd_exit(void)
{
	unregister_command(&cmd_exit);
}

command_initcall(exit_cmd_init);
command_exitcall(exit_cmd_exit);
