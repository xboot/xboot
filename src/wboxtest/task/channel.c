/*
 * wboxtest/task/channel.c
 */

#include <wboxtest.h>

struct wbt_channel_pdata_t
{
	struct channel_t * ch;
	struct channel_t * sync;
};

static void * channel_setup(struct wboxtest_t * wbt)
{
	struct wbt_channel_pdata_t * pdat;

	pdat = malloc(sizeof(struct wbt_channel_pdata_t));
	if(!pdat)
		return NULL;

	pdat->ch = channel_alloc(0);
	pdat->sync = channel_alloc(0);
	if(!pdat->ch || !pdat->sync)
	{
		if(pdat->ch)
			channel_free(pdat->ch);
		if(pdat->sync)
			channel_free(pdat->sync);
		free(pdat);
		return NULL;
	}

	return pdat;
}

static void channel_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_channel_pdata_t * pdat = (struct wbt_channel_pdata_t *)data;

	if(pdat)
	{
		if(pdat->ch)
			channel_free(pdat->ch);
		if(pdat->sync)
			channel_free(pdat->sync);
		free(pdat);
	}
}

static void channel_recv_task(struct task_t * task, void * data)
{
	struct wbt_channel_pdata_t * pdat = (struct wbt_channel_pdata_t *)data;
	int cnt = 10;
	int c = 0;

	while(cnt--)
	{
		channel_recv(pdat->ch, (unsigned char *)&c, sizeof(c));
		assert_equal(c, cnt);
	}
	channel_send(pdat->sync, (unsigned char[]){ 0 }, 1);
}

static void channel_send_task(struct task_t * task, void * data)
{
	struct wbt_channel_pdata_t * pdat = (struct wbt_channel_pdata_t *)data;
	int cnt = 10;

	while(cnt--)
	{
		channel_send(pdat->ch, (unsigned char *)&cnt, sizeof(cnt));
	}
	channel_send(pdat->sync, (unsigned char[]){ 0 }, 1);
}

static void channel_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_channel_pdata_t * pdat = (struct wbt_channel_pdata_t *)data;
	unsigned char buf[2];

	if(pdat)
	{
		task_resume(task_create(scheduler_self(), "channel-recv", channel_recv_task, pdat, 0, 0));
		task_resume(task_create(scheduler_self(), "channel-send", channel_send_task, pdat, 0, 0));
		channel_recv(pdat->sync, buf, 2);
	}
}

static struct wboxtest_t wbt_channel = {
	.group	= "task",
	.name	= "channel",
	.setup	= channel_setup,
	.clean	= channel_clean,
	.run	= channel_run,
};

static __init void channel_wbt_init(void)
{
	register_wboxtest(&wbt_channel);
}

static __exit void channel_wbt_exit(void)
{
	unregister_wboxtest(&wbt_channel);
}

wboxtest_initcall(channel_wbt_init);
wboxtest_exitcall(channel_wbt_exit);
