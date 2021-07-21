/*
 * wboxtest/dma/dmacpy-8bit.c
 */

#include <dma/dma.h>
#include <wboxtest.h>

struct wbt_dmacpy_pdata_t
{
	char * src;
	char * dst;
	size_t size;

	int half;
	int finish;
	int dma;
};

static void * dmacpy_setup(struct wboxtest_t * wbt)
{
	struct wbt_dmacpy_pdata_t * pdat;
	int dma = 0;
	int i;

	if(!dma_is_valid(dma))
		return NULL;

	pdat = malloc(sizeof(struct wbt_dmacpy_pdata_t));
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
	pdat->half = 0;
	pdat->finish = 0;
	pdat->dma = dma;
	dma_stop(pdat->dma);

	return pdat;
}

static void dmacpy_clean(struct wboxtest_t * wbt, void * data)
{
	struct wbt_dmacpy_pdata_t * pdat = (struct wbt_dmacpy_pdata_t *)data;

	if(pdat)
	{
		dma_free_coherent(pdat->dst);
		dma_free_coherent(pdat->src);
		free(pdat);
	}
}

static void dmacpy_half(void * data)
{
	struct wbt_dmacpy_pdata_t * pdat = (struct wbt_dmacpy_pdata_t *)data;
	pdat->half++;
}

static void dmacpy_finish(void * data)
{
	struct wbt_dmacpy_pdata_t * pdat = (struct wbt_dmacpy_pdata_t *)data;
	pdat->finish++;
}

static void dmacpy_run(struct wboxtest_t * wbt, void * data)
{
	struct wbt_dmacpy_pdata_t * pdat = (struct wbt_dmacpy_pdata_t *)data;
	int flag, i;

	if(pdat)
	{
		for(i = 0; i < pdat->size; i++)
		{
			pdat->src[i] = i & 0xff;
			pdat->dst[i] = 0;
		}
		pdat->finish = 0;
		flag = DMA_S_TYPE(DMA_TYPE_MEMTOMEM);
		flag |= DMA_S_SRC_INC(DMA_INCREASE) | DMA_S_DST_INC(DMA_INCREASE);
		flag |= DMA_S_SRC_WIDTH(DMA_WIDTH_8BIT) | DMA_S_DST_WIDTH(DMA_WIDTH_8BIT);
		flag |= DMA_S_SRC_BURST(DMA_BURST_SIZE_1) | DMA_S_DST_BURST(DMA_BURST_SIZE_1);
		flag |= DMA_S_SRC_PORT(0) | DMA_S_DST_PORT(0);
		dma_start(pdat->dma, pdat->src, pdat->dst, pdat->size, flag, dmacpy_half, dmacpy_finish, pdat);
		dma_wait(pdat->dma);
		assert_true(pdat->half > 0);
		assert_true(pdat->finish > 0);
		assert_memory_equal(pdat->src, pdat->dst, pdat->size);
	}
}

static struct wboxtest_t wbt_dmacpy = {
	.group	= "dma",
	.name	= "dmacpy-8bit",
	.setup	= dmacpy_setup,
	.clean	= dmacpy_clean,
	.run	= dmacpy_run,
};

static __init void dmacpy_wbt_init(void)
{
	register_wboxtest(&wbt_dmacpy);
}

static __exit void dmacpy_wbt_exit(void)
{
	unregister_wboxtest(&wbt_dmacpy);
}

wboxtest_initcall(dmacpy_wbt_init);
wboxtest_exitcall(dmacpy_wbt_exit);
