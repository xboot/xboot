#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	DMA_BIDIRECTIONAL	= 0,
	DMA_TO_DEVICE		= 1,
	DMA_FROM_DEVICE		= 2,
};

void * dma_alloc_coherent(unsigned long size);
void dma_free_coherent(void * addr);
void * dma_alloc_noncoherent(unsigned long size);
void dma_free_noncoherent(void * addr);
void dma_cache_sync(void * addr, unsigned long size, int dir);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
