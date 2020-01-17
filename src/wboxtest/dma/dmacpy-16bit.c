/*
 * wboxtest/dma/dmacpy-16bit.c
 */

#include <dma/dma.h>
#include <wboxtest.h>

struct wbt_dmacpy_pdata_t
{
	char * src;
	char * dst;
	size_t size;

	int complete;
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
	pdat->complete = 0;
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

static void dmacpy_complete(void * data)
{
	struct wbt_dmacpy_pdata_t * pdat = (struct wbt_dmacpy_pdata_t *)data;
	pdat->complete = 1;
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
		pdat->complete = 0;
		flag = DMA_S_TYPE(DMA_TYPE_MEMTOMEM);
		flag |= DMA_S_SRC_INC(DMA_INCREASE) | DMA_S_DST_INC(DMA_INCREASE);
		flag |= DMA_S_SRC_WIDTH(DMA_WIDTH_16BIT) | DMA_S_DST_WIDTH(DMA_WIDTH_16BIT);
		flag |= DMA_S_SRC_BURST(DMA_BURST_SIZE_1) | DMA_S_DST_BURST(DMA_BURST_SIZE_1);
		flag |= DMA_S_SRC_PORT(0) | DMA_S_DST_PORT(0);
		dma_start(pdat->dma, pdat->src, pdat->dst, pdat->size, flag, dmacpy_complete, pdat);
		dma_wait(pdat->dma);
		assert_true(pdat->complete);
		assert_memory_equal(pdat->src, pdat->dst, pdat->size);
	}
}

static struct wboxtest_t wbt_dmacpy = {
	.group	= "dma",
	.name	= "dmacpy-16bit",
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
