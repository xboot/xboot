#ifndef __ARM32_CACHE_H__
#define __ARM32_CACHE_H__

#include <xboot.h>

#ifdef __cplusplus
extern "C" {
#endif

void dma_clean_range(unsigned long start, unsigned long end);
void dma_flush_range(unsigned long start, unsigned long end);
void dma_inv_range(unsigned long start, unsigned long end);
void mmu_cache_on(void);
void mmu_cache_off(void);
void mmu_cache_flush(void);
void mmu_cache_invalidate(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_CACHE_H__ */
