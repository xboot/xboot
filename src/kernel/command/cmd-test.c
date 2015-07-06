/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [arg ...]\r\n");
}

static int test(int argc, char ** argv)
{
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "test command for debug",
	.usage	= usage,
	.exec	= test,
};

static __init void test_cmd_init(void)
{
	command_register(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	command_unregister(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
