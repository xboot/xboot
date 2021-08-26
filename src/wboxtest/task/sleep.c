/*
 * wboxtest/task/sleep.c
 */

#include <wboxtest.h>

struct wbt_sleep_pdata_t
{
	struct waiter_t w;
};

static void * sleep_setup(struct wboxtest_t * wbt)
{
	struct wbt_sleep_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_sleep_pdata_t));
	if(!pdat)
		return NULL;
	waiter_init(&pdat->w);

	return pdat;
}

static void sleep_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_sleep_pdata_t * pdat = (struct wbt_sleep_pdata_t *)data;

	if(pdat)
		free(pdat);
}

static void sleep_task(struct task_t * task, void * data)
{
	struct wbt_sleep_pdata_t * pdat = (struct wbt_sleep_pdata_t *)data;
	int cnt = 10;

	while(cnt--)
	{
		wboxtest_print("sleep task count = %d\r\n", cnt);
		msleep(500);
	}
	waiter_sub(&pdat->w, 1);
}

static void sleep_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_sleep_pdata_t * pdat = (struct wbt_sleep_pdata_t *)data;

	if(pdat)
	{
		waiter_add(&pdat->w, 1);
		task_create(scheduler_self(), "sleep-task", NULL, NULL, sleep_task, pdat, 0, 0);
		waiter_wait(&pdat->w);
	}
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
