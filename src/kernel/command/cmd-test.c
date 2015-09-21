/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

#include <queue.h>
struct queue_t * q;

struct pmsg_t {
	int value;
};

static int do_test(int argc, char ** argv)
{
	int i;

	for(i = 1; i < argc; i++)
	{
		if( !strcmp(argv[i],"init") )
		{
			q = queue_alloc();
			printf("init\r\n");
		}
		else if( !strcmp(argv[i],"exit") )
		{
			queue_free(q);
			printf("exit\r\n");
		}
		else if( !strcmp(argv[i],"clear") )
		{
			queue_clear(q);
			printf("clear\r\n");
		}
		else if( !strcmp(argv[i],"push") )
		{
			struct pmsg_t * m = malloc(sizeof(struct pmsg_t));
			m->value = strtoul(argv[i+1], NULL, 0);
			queue_push(q, m);
			printf("push: %d\r\n", m->value);
			i++;
		}
		else if( !strcmp(argv[i],"pop") )
		{
			struct pmsg_t * m = queue_pop(q);
			if(m)
			{
				printf("pop: %d\r\n", m->value);
				free(m);
			}
			else
				printf("pop: none\r\n");
		}
		else if( !strcmp(argv[i],"peek") )
		{
			struct pmsg_t * m = queue_peek(q);
			if(m)
				printf("peek: %d\r\n", m->value);
			else
				printf("peek: none\r\n");
		}
	}

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
