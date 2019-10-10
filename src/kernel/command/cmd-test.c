/*
 * kernel/command/cmd-test.c
 */

#include <xboot.h>
#include <command/command.h>
#include <xui/xui.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

static void demo_func(struct xui_context_t * ctx)
{
	struct color_t c;
	color_init(&c, 90, 50, 40, 255);
	xui_push_rectangle(ctx, 10, 10, 100, 100, 20, 5, &c);
}

static void xui_demo_task(struct task_t * task, void * data)
{
	struct xui_context_t * ctx = xui_context_alloc(NULL, NULL, NULL);
	xui_loop(ctx, demo_func);
	xui_context_free(ctx);
}

static int do_test(int argc, char ** argv)
{
	struct task_t * task;

	task = task_create(NULL, "demo", xui_demo_task, NULL, 0, 0);
	task_resume(task);
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
