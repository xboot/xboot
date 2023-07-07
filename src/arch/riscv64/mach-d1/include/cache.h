#ifndef __RISCV64_CACHE_H__
#define __RISCV64_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sizes.h>
#include <barrier.h>
#include <riscv64.h>
#include <dma/dma.h>

void dcache_wb_range(unsigned long start, unsigned long end);
void dcache_inv_range(unsigned long start, unsigned long end);
void dcache_wbinv_range(unsigned long start, unsigned long end);

void mmu_setup(void);
void mmu_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_CACHE_H__ */
