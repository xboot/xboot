/*
 * kernel/command/cmd-ilda.c
 */

#include <xboot.h>
#include <laserscan/laserscan.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    ilda <filename> [count]\r\n");
}

static int do_ilda(int argc, char ** argv)
{
	struct laserscan_t * l = search_first_laserscan();
	char fpath[VFS_MAX_PATH];
	int count = 1;
	int i;

	if(argc < 2)
	{
		usage();
		return -1;
	}
	if(argc == 3)
		count = strtoul(argv[2], NULL, 0);

	for(i = 0; i < count; i++)
	{
		shell_realpath(argv[1], fpath);
		laserscan_load_ilda(l, fpath);
	}
	return 0;
}

static struct command_t cmd_ilda = {
	.name	= "ilda",
	.desc	= "international laser display association",
	.usage	= usage,
	.exec	= do_ilda,
};

static __init void ilda_cmd_init(void)
{
	register_command(&cmd_ilda);
}

static __exit void ilda_cmd_exit(void)
{
	unregister_command(&cmd_ilda);
}

command_initcall(ilda_cmd_init);
command_exitcall(ilda_cmd_exit);
