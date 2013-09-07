/*
 * cache.c
 */

#include <cache.h>

#if __ARM_ARCH__ == 4
extern void v4_dma_clean_range(unsigned long start, unsigned long end);
extern void v4_dma_flush_range(unsigned long start, unsigned long end);
extern void v4_dma_inv_range(unsigned long start, unsigned long end);
extern void v4_mmu_cache_on(void);
extern void v4_mmu_cache_off(void);
extern void v4_mmu_cache_flush(void);
#endif
#if __ARM_ARCH__ == 5
extern void v5_dma_clean_range(unsigned long start, unsigned long end);
extern void v5_dma_flush_range(unsigned long start, unsigned long end);
extern void v5_dma_inv_range(unsigned long start, unsigned long end);
extern void v5_mmu_cache_on(void);
extern void v5_mmu_cache_off(void);
extern void v5_mmu_cache_flush(void);
#endif
#if __ARM_ARCH__ == 6
extern void v6_dma_clean_range(unsigned long start, unsigned long end);
extern void v6_dma_flush_range(unsigned long start, unsigned long end);
extern void v6_dma_inv_range(unsigned long start, unsigned long end);
extern void v6_mmu_cache_on(void);
extern void v6_mmu_cache_off(void);
extern void v6_mmu_cache_flush(void);
#endif
#if __ARM_ARCH__ == 7
extern void v7_dma_clean_range(unsigned long start, unsigned long end);
extern void v7_dma_flush_range(unsigned long start, unsigned long end);
extern void v7_dma_inv_range(unsigned long start, unsigned long end);
extern void v7_mmu_cache_on(void);
extern void v7_mmu_cache_off(void);
extern void v7_mmu_cache_flush(void);
extern void v7_mmu_cache_invalidate(void);
#endif

void __dma_clean_range(unsigned long start, unsigned long end)
{
#if __ARM_ARCH__ == 4
	v4_dma_clean_range(start, end);
#endif
#if __ARM_ARCH__ == 5
	v5_dma_clean_range(start, end);
#endif
#if __ARM_ARCH__ == 6
	v6_dma_clean_range(start, end);
#endif
#if __ARM_ARCH__ == 7
	v7_dma_clean_range(start, end);
#endif
}

void __dma_flush_range(unsigned long start, unsigned long end)
{
#if __ARM_ARCH__ == 4
	v4_dma_flush_range(start, end);
#endif
#if __ARM_ARCH__ == 5
	v5_dma_flush_range(start, end);
#endif
#if __ARM_ARCH__ == 6
	v6_dma_flush_range(start, end);
#endif
#if __ARM_ARCH__ == 7
	v7_dma_flush_range(start, end);
#endif
}

void __dma_inv_range(unsigned long start, unsigned long end)
{
#if __ARM_ARCH__ == 4
	v4_dma_inv_range(start, end);
#endif
#if __ARM_ARCH__ == 5
	v5_dma_inv_range(start, end);
#endif
#if __ARM_ARCH__ == 6
	v6_dma_inv_range(start, end);
#endif
#if __ARM_ARCH__ == 7
	v7_dma_inv_range(start, end);
#endif
}

void __mmu_cache_on(void)
{
#if __ARM_ARCH__ == 4
	v4_mmu_cache_on();
#endif
#if __ARM_ARCH__ == 5
	v5_mmu_cache_on();
#endif
#if __ARM_ARCH__ == 6
	v6_mmu_cache_on();
#endif
#if __ARM_ARCH__ == 7
	v7_mmu_cache_on();
#endif
}

void __mmu_cache_off(void)
{
#if __ARM_ARCH__ == 4
	v4_mmu_cache_off();
#endif
#if __ARM_ARCH__ == 5
	v5_mmu_cache_off();
#endif
#if __ARM_ARCH__ == 6
	v6_mmu_cache_off();
#endif
#if __ARM_ARCH__ == 7
	v7_mmu_cache_off();
#endif
}

void __mmu_cache_flush(void)
{
#if __ARM_ARCH__ == 4
	v4_mmu_cache_flush();
#endif
#if __ARM_ARCH__ == 5
	v5_mmu_cache_flush();
#endif
#if __ARM_ARCH__ == 6
	v6_mmu_cache_flush();
#endif
#if __ARM_ARCH__ == 7
	v7_mmu_cache_flush();
#endif
}

void __mmu_cache_invalidate(void)
{
#if __ARM_ARCH__ >= 4 && __ARM_ARCH__ <= 6
	asm volatile("mcr p15, 0, %0, c7, c6, 0\n" : : "r"(0));
#endif
#if __ARM_ARCH__ == 7
	v7_mmu_cache_invalidate();
#endif
}
