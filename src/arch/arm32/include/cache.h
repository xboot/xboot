#ifndef __ARM32_CACHE_H__
#define __ARM32_CACHE_H__

#include <xconfigs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __dma_clean_range(unsigned long start, unsigned long end);
void __dma_flush_range(unsigned long start, unsigned long end);
void __dma_inv_range(unsigned long start, unsigned long end);
void __mmu_cache_on(void);
void __mmu_cache_off(void);
void __mmu_cache_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_CACHE_H__ */
