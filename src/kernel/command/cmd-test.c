/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

void f1_task(struct task_t * self, void * data)
{
    int count = 10;
	while(count--)
    {
        printf("f1_task: %lu\r\n", count);
        mdelay(100);
        task_yield(self);
    }
}

void f2_task(struct task_t * self, void * data)
{
    int count = 30;
	while(1)
    {
        printf("f2_task: %lu\r\n", count--);
        mdelay(100);
        task_yield(self);
    }
}

void f3_task(struct task_t * self, void * data)
{
    int count = 50;
	while(1)
    {
        printf("f3_task: %lu\r\n", count--);
        mdelay(100);
        task_yield(self);
    }
}

struct scheduler_t * s;
struct task_t * t1;
struct task_t * t2;
struct task_t * t3;

static int do_test(int argc, char ** argv)
{
	s = scheduler_alloc();
	t1 = task_create(s, "f1_task", f1_task, 0, 0, 0);
	t2 = task_create(s, "f2_task", f2_task, 0, 0, 0);
	t3 = task_create(s, "f3_task", f3_task, 0, 0, 0);

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
