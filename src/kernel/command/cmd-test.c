/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static int do_test(int argc, char ** argv)
{
	char json[256];
	int length;

	length = sprintf(json, "{\"sdhci-spi@0\":{\"spi-bus\":\"spi-gpio.0\"}}");
	printf("%s\r\n", json);
	probe_device(json, length, NULL);
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
