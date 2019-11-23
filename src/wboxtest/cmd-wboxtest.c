/*
 * wboxtest/cmd-wboxtest.c
 */

#include <wboxtest.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    wboxtest [group] [name] [-c count]\r\n");
	printf("    wboxtest -l\r\n");
}

static int do_wboxtest(int argc, char ** argv)
{
	const char * group = NULL;
	const char * name = NULL;
	int count = 1;
	int i, index = 0;

	if((argc == 2) && !strcmp(argv[1], "-l"))
	{
		wboxtest_list();
	}
	else
	{
		for(i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i], "-c") && (argc > i + 1))
			{
				count = strtoul(argv[i + 1], NULL, 0);
				i++;
			}
			else if(*argv[i] == '-')
			{
				usage();
				return -1;
			}
			else if(*argv[i] != '-' && strcmp(argv[i], "-") != 0)
			{
				if(index == 0)
					group = argv[i];
				else if(index == 1)
					name = argv[i];
				else
				{
					usage();
					return -1;
				}
				index++;
			}
		}
		if(count > 0)
		{
			if(!group && !name)
				wboxtest_run_all(count);
			else if(group && !name)
				wboxtest_run_group(group, count);
			else if(group && name)
				wboxtest_run_group_name(group, name, count);
		}
	}
	return 0;
}

static struct command_t cmd_wboxtest = {
	.name	= "wboxtest",
	.desc	= "whitebox test for some modules",
	.usage	= usage,
	.exec	= do_wboxtest,
};

static __init void wboxtest_cmd_init(void)
{
	register_command(&cmd_wboxtest);
}

static __exit void wboxtest_cmd_exit(void)
{
	unregister_command(&cmd_wboxtest);
}

command_initcall(wboxtest_cmd_init);
command_exitcall(wboxtest_cmd_exit);
