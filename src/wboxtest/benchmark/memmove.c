/*
 * wboxtest/benchmark/memmove.c
 */

#include <wboxtest.h>

struct wbt_memmove_pdata_t
{
	char * src;
	char * dst;
	size_t size;

	ktime_t t1;
	ktime_t t2;
	int calls;
};

static void * memmove_setup(struct wboxtest_t * wbt)
{
	struct wbt_memmove_pdata_t * pdat;
	int i;

	pdat = malloc(sizeof(struct wbt_memmove_pdata_t));
	if(!pdat)
		return NULL;

	pdat->size = SZ_1M;
	pdat->src = malloc(pdat->size);
	pdat->dst = malloc(pdat->size);
	if(!pdat->src || !pdat->dst)
	{
		free(pdat->src);
		free(pdat->dst);
		free(pdat);
		return NULL;
	}
	for(i = 0; i < pdat->size; i++)
	{
		pdat->src[i] = i & 0xff;
		pdat->dst[i] = 0;
	}

	return pdat;
}

static void memmove_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memmove_pdata_t * pdat = (struct wbt_memmove_pdata_t *)data;

	if(pdat)
	{
		free(pdat->dst);
		free(pdat->src);
		free(pdat);
	}
}

static void memmove_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memmove_pdata_t * pdat = (struct wbt_memmove_pdata_t *)data;
	char buf[32];

	if(pdat)
	{
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			pdat->calls++;
			memmove(pdat->dst, pdat->src, pdat->size);
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Bandwidth: %s/s\r\n", ssize(buf, (double)(pdat->calls * pdat->size) * 1000.0 / ktime_ms_delta(pdat->t2, pdat->t1)));
	}
}

static struct wboxtest_t wbt_memmove = {
	.group	= "benchmark",
	.name	= "memmove",
	.setup	= memmove_setup,
	.clean	= memmove_clean,
	.run	= memmove_run,
};

static __init void memmove_wbt_init(void)
{
	register_wboxtest(&wbt_memmove);
}

static __exit void memmove_wbt_exit(void)
{
	unregister_wboxtest(&wbt_memmove);
}

wboxtest_initcall(memmove_wbt_init);
wboxtest_exitcall(memmove_wbt_exit);
