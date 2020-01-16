/*
 * wboxtest/dma/benchmark.c
 */

#include <dma/dma.h>
#include <wboxtest.h>

struct wbt_benchmark_pdata_t
{
	char * src;
	char * dst;
	size_t size;

	ktime_t t1;
	ktime_t t2;
	int calls;
	int dma;
};

static void * benchmark_setup(struct wboxtest_t * wbt)
{
	struct wbt_benchmark_pdata_t * pdat;
	int dma = 0;
	int i;

	if(!dma_is_valid(dma))
		return NULL;

	pdat = malloc(sizeof(struct wbt_benchmark_pdata_t));
	if(!pdat)
		return NULL;

	pdat->size = SZ_256K;
	pdat->src = dma_alloc_coherent(pdat->size);
	pdat->dst = dma_alloc_coherent(pdat->size);
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
	pdat->dma = dma;
	dma_stop(pdat->dma);

	return pdat;
}

static void benchmark_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_benchmark_pdata_t * pdat = (struct wbt_benchmark_pdata_t *)data;

	if(pdat)
	{
		dma_free_coherent(pdat->dst);
		dma_free_coherent(pdat->src);
		free(pdat);
	}
}

static void benchmark_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_benchmark_pdata_t * pdat = (struct wbt_benchmark_pdata_t *)data;
	char buf[32];
	int flag;
	if(pdat)
	{
		flag = DMA_S_TYPE(DMA_TYPE_MEMTOMEM);
		flag |= DMA_S_SRC_INC(DMA_INCREASE) | DMA_S_DST_INC(DMA_INCREASE);
		flag |= DMA_S_SRC_WIDTH(DMA_WIDTH_8BIT) | DMA_S_DST_WIDTH(DMA_WIDTH_8BIT);
		flag |= DMA_S_SRC_BURST(DMA_BURST_SIZE_1) | DMA_S_DST_BURST(DMA_BURST_SIZE_1);
		flag |= DMA_S_SRC_PORT(0) | DMA_S_DST_PORT(0);
		pdat->calls = 0;
		pdat->t2 = pdat->t1 = ktime_get();
		do {
			pdat->calls++;
			dma_start(pdat->dma, pdat->src, pdat->dst, pdat->size, flag, NULL, NULL);
			dma_wait(pdat->dma);
			pdat->t2 = ktime_get();
		} while(ktime_before(pdat->t2, ktime_add_ms(pdat->t1, 2000)));
		wboxtest_print(" Bandwidth: %s/s\r\n", ssize(buf, (double)(pdat->calls * pdat->size) * 1000.0 / ktime_ms_delta(pdat->t2, pdat->t1)));
	}
}

static struct wboxtest_t wbt_benchmark = {
	.group	= "dma",
	.name	= "benchmark",
	.setup	= benchmark_setup,
	.clean	= benchmark_clean,
	.run	= benchmark_run,
};

static __init void benchmark_wbt_init(void)
{
	register_wboxtest(&wbt_benchmark);
}

static __exit void benchmark_wbt_exit(void)
{
	unregister_wboxtest(&wbt_benchmark);
}

wboxtest_initcall(benchmark_wbt_init);
wboxtest_exitcall(benchmark_wbt_exit);
