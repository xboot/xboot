#ifndef __ARM32_CP15_H__
#define __ARM32_CP15_H__

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void irq_enable(void);
void irq_disable(void);
void fiq_enable(void);
void fiq_disable(void);
void icache_enable(void);
void icache_disable(void);
int icache_status(void);
void dcache_enable(void);
void dcache_disable(void);
int dcache_status(void);
void wbuffer_enable(void);
void wbuffer_disable(void);
int wbuffer_status(void);
void mmu_enable(void);
void mmu_disable(void);
int mmu_status(void);
void vic_enable(void);
void vic_disable(void);
int vic_status(void);
void branch_enable(void);
void branch_disable(void);
int branch_status(void);
void ttb_set(u32_t base);
u32_t ttb_get(void);
void domain_set(u32_t domain);
u32_t domain_get(void);
void tlb_invalidate(void);
u32_t get_cpuid(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_CP15_H__ */
