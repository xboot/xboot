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

void cache_flush_range(unsigned long start, unsigned long stop);
void cache_inv_range(unsigned long start, unsigned long stop);

void mmu_setup(void);
void mmu_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_CACHE_H__ */
