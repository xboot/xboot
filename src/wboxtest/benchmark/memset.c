/*
 * wboxtest/benchmark/memset.c
 */

#include <wboxtest.h>

struct wbt_memset_pdata_t
{
	char * src;
	size_t size;

	ktime_t t1;
	ktime_t t2;
	int calls;
};

static void * memset_setup(struct wboxtest_t * wbt)
{
	struct wbt_memset_pdata_t * pdat;
	int i;

	pdat = malloc(sizeof(struct wbt_memset_pdata_t));
	if(!pdat)
		return NULL;

	pdat->size = SZ_1M;
	pdat->src = malloc(pdat->size);
	if(!pdat->src)
	{
		free(pdat->src);
		free(pdat);
		return NULL;
	}
	for(i = 0; i < pdat->size; i++)
	{
		pdat->src[i] = 0x55;
	}

	return pdat;
}

static void memset_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memset_pdata_t * pdat = (struct wbt_memset_pdata_t *)data;

	if(pdat)
	{
		free(pdat->src);
		free(pdat);
	}
}

static void memset_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memset_pdata_t * pdat = (struct wbt_memset_pdata_t *)data;
	char buf[32];

	if(pdat)
	{
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			pdat->calls++;
			memset(pdat->src, pdat->calls, pdat->size);
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Bandwidth: %s/s\r\n", ssize(buf, (double)(pdat->calls * pdat->size) * 1000.0 / ktime_ms_delta(pdat->t2, pdat->t1)));
	}
}

static struct wboxtest_t wbt_memset = {
	.group	= "benchmark",
	.name	= "memset",
	.setup	= memset_setup,
	.clean	= memset_clean,
	.run	= memset_run,
};

static __init void memset_wbt_init(void)
{
	register_wboxtest(&wbt_memset);
}

static __exit void memset_wbt_exit(void)
{
	unregister_wboxtest(&wbt_memset);
}

wboxtest_initcall(memset_wbt_init);
wboxtest_exitcall(memset_wbt_exit);
