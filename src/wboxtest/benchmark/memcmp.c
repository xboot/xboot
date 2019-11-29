/*
 * wboxtest/benchmark/memcmp.c
 */

#include <wboxtest.h>

struct wbt_memcmp_pdata_t
{
	char * src;
	char * dst;
	size_t size;

	ktime_t t1;
	ktime_t t2;
	int calls;
};

static void * memcmp_setup(struct wboxtest_t * wbt)
{
	struct wbt_memcmp_pdata_t * pdat;
	int i;

	pdat = malloc(sizeof(struct wbt_memcmp_pdata_t));
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
		pdat->src[i] = 0x55;
		pdat->dst[i] = 0x55;
	}

	return pdat;
}

static void memcmp_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memcmp_pdata_t * pdat = (struct wbt_memcmp_pdata_t *)data;

	if(pdat)
	{
		free(pdat->dst);
		free(pdat->src);
		free(pdat);
	}
}

static void memcmp_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_memcmp_pdata_t * pdat = (struct wbt_memcmp_pdata_t *)data;
	char buf[32];

	if(pdat)
	{
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			pdat->calls++;
			memcmp(pdat->dst, pdat->src, pdat->size);
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Bandwidth: %s/s\r\n", ssize(buf, (double)(pdat->calls * pdat->size) * 1000.0 / ktime_ms_delta(pdat->t2, pdat->t1)));
	}
}

static struct wboxtest_t wbt_memcmp = {
	.group	= "benchmark",
	.name	= "memcmp",
	.setup	= memcmp_setup,
	.clean	= memcmp_clean,
	.run	= memcmp_run,
};

static __init void memcmp_wbt_init(void)
{
	register_wboxtest(&wbt_memcmp);
}

static __exit void memcmp_wbt_exit(void)
{
	unregister_wboxtest(&wbt_memcmp);
}

wboxtest_initcall(memcmp_wbt_init);
wboxtest_exitcall(memcmp_wbt_exit);
