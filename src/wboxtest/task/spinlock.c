/*
 * wboxtest/task/spinlock.c
 */

#include <wboxtest.h>

struct wbt_spinlock_pdata_t
{
	struct waiter_t w;
	spinlock_t lock;
};

static void * spinlock_setup(struct wboxtest_t * wbt)
{
	struct wbt_spinlock_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_spinlock_pdata_t));
	if(!pdat)
		return NULL;
	waiter_init(&pdat->w);
	spin_lock_init(&pdat->lock);

	return pdat;
}

static void spinlock_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_spinlock_pdata_t * pdat = (struct wbt_spinlock_pdata_t *)data;

	if(pdat)
		free(pdat);
}

static void spinlock_task(struct task_t * task, void * data)
{
	struct wbt_spinlock_pdata_t * pdat = (struct wbt_spinlock_pdata_t *)data;
	int cnt = 10;

	while(cnt--)
	{
		spin_lock(&pdat->lock);
		wboxtest_print("[%s]task running %d\r\n", task->name, cnt);
		spin_unlock(&pdat->lock);
		task_yield();
	}
	waiter_sub(&pdat->w, 1);
}

static void spinlock_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_spinlock_pdata_t * pdat = (struct wbt_spinlock_pdata_t *)data;
	char name[128];

	if(pdat)
	{
		for(int i = 0; i < 10; i++)
		{
			waiter_add(&pdat->w, 1);
			sprintf(name, "spinlock-task-%02d", i);
			task_create(scheduler_self(), name, NULL, NULL, spinlock_task, pdat, 0, 0);
		}
		waiter_wait(&pdat->w);
	}
}

static struct wboxtest_t wbt_spinlock = {
	.group	= "task",
	.name	= "spinlock",
	.setup	= spinlock_setup,
	.clean	= spinlock_clean,
	.run	= spinlock_run,
};

static __init void spinlock_wbt_init(void)
{
	register_wboxtest(&wbt_spinlock);
}

static __exit void spinlock_wbt_exit(void)
{
	unregister_wboxtest(&wbt_spinlock);
}

wboxtest_initcall(spinlock_wbt_init);
wboxtest_exitcall(spinlock_wbt_exit);
