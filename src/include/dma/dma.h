#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void * dma_alloc(size_t size);
void * dma_zalloc(size_t size);
void dma_free(void * ptr);

void do_init_dma_pool(void);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
