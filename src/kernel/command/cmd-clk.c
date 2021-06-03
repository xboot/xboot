/*
 * kernel/command/cmd-clk.c
 */

#include <xboot.h>
#include <clk/clk.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    clk [name ...]\r\n");
}

static void clk_dump(char * parent, int level)
{
	struct device_t * pos, * n;
	struct clk_t * clk;
	const char * pname;
	char * name;
	u64_t rate;
	int i;

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_CLK], head)
	{
		if((clk = (struct clk_t *)(pos->priv)))
		{
			name = clk->name;
			pname = clk_get_parent(name);
			if((parent && pname && (strcmp(parent, pname) == 0)) || (!parent && !pname))
			{
				rate = clk_get_rate(name);
				for(i = 0; i < level; i++)
					printf("    ");
				printf("%*s %6Ld.%06LdMHZ %4d\r\n", -(64 - level * 4), name, rate / (u64_t)(1000 * 1000), rate % (u64_t)(1000 * 1000), clk_status(name) ? 1 : 0);
				clk_dump(clk->name, level + 1);
			}
		}
	}
}

static int do_clk(int argc, char ** argv)
{
	u64_t rate;
	int i;

	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(search_clk(argv[i]))
			{
				rate = clk_get_rate(argv[i]);
				printf("%*s %6Ld.%06LdMHZ %4d\r\n", -(64 - 0 * 4), argv[i], rate / (u64_t)(1000 * 1000), rate % (u64_t)(1000 * 1000), clk_status(argv[i]) ? 1 : 0);
				clk_dump(argv[i], 1);
			}
		}
	}
	else
	{
		clk_dump(NULL, 0);
	}
	return 0;
}

static struct command_t cmd_clk = {
	.name	= "clk",
	.desc	= "dump clock in a tree-like format",
	.usage	= usage,
	.exec	= do_clk,
};

static __init void clk_cmd_init(void)
{
	register_command(&cmd_clk);
}

static __exit void clk_cmd_exit(void)
{
	unregister_command(&cmd_clk);
}

command_initcall(clk_cmd_init);
command_exitcall(clk_cmd_exit);
