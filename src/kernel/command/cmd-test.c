/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>
#include <xboot/task.h>
/*#include <context.h>

static void demo_platform_context_test_func1(struct transfer_t from)
{
    void ** contexts = (void **)from.priv;

    contexts[0] = from.ctx;
    from.ctx = contexts[2];

    size_t count = 10;
	while (count--)
    {
        printf("func1: %lu\r\n", count);
        from = jump_context(from.ctx, contexts);
    }
	jump_context(contexts[0], NULL);
}
static void demo_platform_context_test_func2(struct transfer_t from)
{
    void ** contexts = (void **)from.priv;

    size_t count = 10;
	while (count--)
    {
        printf("func2: %lu\r\n", count);
        from = jump_context(from.ctx, contexts);
    }
	jump_context(contexts[0], NULL);
}
static void demo_platform_context_test()
{
    static void * contexts[3];
	static char stacks1[8192];
	static char stacks2[8192];

    contexts[1] = make_context(stacks1, sizeof(stacks1), demo_platform_context_test_func1);
    contexts[2] = make_context(stacks2, sizeof(stacks2), demo_platform_context_test_func2);

    jump_context(contexts[1], contexts);
}

int demo_platform_context_main(int argc, char * argv[])
{
    demo_platform_context_test();
    return 0;
}
*/

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

void f1_task(struct scheduler_t * s, void * data)
{
    int count = 10;
	while(count--)
    {
        printf("f1_task: %lu\r\n", count);
        mdelay(100);
        task_yield(s);
    }
}

void f2_task(struct scheduler_t * s, void * data)
{
    int count = 30;
	while(count--)
    {
        printf("f2_task: %lu\r\n", count);
        mdelay(100);
        task_yield(s);
    }
}

void f3_task(struct scheduler_t * s, void * data)
{
    int count = 50;
	while(1)
    {
        printf("f3_task: %lu\r\n", count--);
        mdelay(100);
        task_yield(s);
    }
}

struct scheduler_t * s;
struct task_t * t1;
struct task_t * t2;
struct task_t * t3;

static int do_test(int argc, char ** argv)
{
	s = scheduler_alloc();
	t1 = task_create(s, "f1_task", f1_task, 0, 0);
	t2 = task_create(s, "f2_task", f2_task, 0, 0);
	t3 = task_create(s, "f3_task", f3_task, 0, 0);

	scheduler_loop(s);

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
