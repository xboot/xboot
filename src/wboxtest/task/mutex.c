/*
 * wboxtest/task/mutex.c
 */

#include <wboxtest.h>

struct wbt_mutex_pdata_t
{
	struct waiter_t w;
	struct mutex_t m;
};

static void * mutex_setup(struct wboxtest_t * wbt)
{
	struct wbt_mutex_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_mutex_pdata_t));
	if(!pdat)
		return NULL;
	waiter_init(&pdat->w);
	mutex_init(&pdat->m);

	return pdat;
}

static void mutex_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_mutex_pdata_t * pdat = (struct wbt_mutex_pdata_t *)data;

	if(pdat)
		free(pdat);
}

static void mutex_task(struct task_t * task, void * data)
{
	struct wbt_mutex_pdata_t * pdat = (struct wbt_mutex_pdata_t *)data;
	int cnt = 10;

	while(cnt--)
	{
		mutex_lock(&pdat->m);
		wboxtest_print("[%s]task running %d\r\n", task->name, cnt);
		mutex_unlock(&pdat->m);
		task_yield();
	}
	waiter_sub(&pdat->w, 1);
}

static void mutex_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_mutex_pdata_t * pdat = (struct wbt_mutex_pdata_t *)data;
	char name[128];

	if(pdat)
	{
		for(int i = 0; i < 10; i++)
		{
			waiter_add(&pdat->w, 1);
			sprintf(name, "mutex-task-%02d", i);
			task_create(scheduler_self(), name, NULL, NULL, mutex_task, pdat, 0, 0);
		}
		waiter_wait(&pdat->w);
	}
}

static struct wboxtest_t wbt_mutex = {
	.group	= "task",
	.name	= "mutex",
	.setup	= mutex_setup,
	.clean	= mutex_clean,
	.run	= mutex_run,
};

static __init void mutex_wbt_init(void)
{
	register_wboxtest(&wbt_mutex);
}

static __exit void mutex_wbt_exit(void)
{
	unregister_wboxtest(&wbt_mutex);
}

wboxtest_initcall(mutex_wbt_init);
wboxtest_exitcall(mutex_wbt_exit);
