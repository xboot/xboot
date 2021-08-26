/*
 * wboxtest/task/waiter.c
 */

#include <wboxtest.h>

struct wbt_waiter_pdata_t
{
	struct waiter_t w;
};

static void * waiter_setup(struct wboxtest_t * wbt)
{
	struct wbt_waiter_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_waiter_pdata_t));
	if(!pdat)
		return NULL;
	waiter_init(&pdat->w);

	return pdat;
}

static void waiter_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_waiter_pdata_t * pdat = (struct wbt_waiter_pdata_t *)data;

	if(pdat)
		free(pdat);
}

static void waiter_task(struct task_t * task, void * data)
{
	struct wbt_waiter_pdata_t * pdat = (struct wbt_waiter_pdata_t *)data;
	int cnt = 10;

	while(cnt--)
	{
		msleep(100);
	}
	wboxtest_print("%s task exit\r\n", task->name);
	waiter_sub(&pdat->w, 1);
}

static void waiter_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_waiter_pdata_t * pdat = (struct wbt_waiter_pdata_t *)data;
	char name[128];

	if(pdat)
	{
		for(int i = 0; i < 10; i++)
		{
			waiter_add(&pdat->w, 1);
			sprintf(name, "waiter-task-%02d", i);
			task_create(NULL, name, NULL, NULL, waiter_task, pdat, 0, 0);
		}
		waiter_wait(&pdat->w);
	}
}

static struct wboxtest_t wbt_waiter = {
	.group	= "task",
	.name	= "waiter",
	.setup	= waiter_setup,
	.clean	= waiter_clean,
	.run	= waiter_run,
};

static __init void waiter_wbt_init(void)
{
	register_wboxtest(&wbt_waiter);
}

static __exit void waiter_wbt_exit(void)
{
	unregister_wboxtest(&wbt_waiter);
}

wboxtest_initcall(waiter_wbt_init);
wboxtest_exitcall(waiter_wbt_exit);
