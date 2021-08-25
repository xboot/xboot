/*
 * wboxtest/task/sleep.c
 */

#include <wboxtest.h>

static void * sleep_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void sleep_clean(struct wboxtest_t * wbt, void * data)
{
}

static void sleep_task(struct task_t * task, void * data)
{
	int cnt = 10;

	while(cnt--)
	{
		wboxtest_print("sleep task count = %d\r\n", cnt);
		msleep(500);
	}
}

static void sleep_run(struct wboxtest_t * wbt, void * data)
{
	task_create(NULL, "sleep-task", NULL, NULL, sleep_task, NULL, 0, 0);
	msleep(5000);
}

static struct wboxtest_t wbt_sleep = {
	.group	= "task",
	.name	= "sleep",
	.setup	= sleep_setup,
	.clean	= sleep_clean,
	.run	= sleep_run,
};

static __init void sleep_wbt_init(void)
{
	register_wboxtest(&wbt_sleep);
}

static __exit void sleep_wbt_exit(void)
{
	unregister_wboxtest(&wbt_sleep);
}

wboxtest_initcall(sleep_wbt_init);
wboxtest_exitcall(sleep_wbt_exit);
