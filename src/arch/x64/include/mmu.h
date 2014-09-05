#ifndef __X64_MMU_H__
#define __X64_MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void mmu_setup(struct machine_t * mach);
void * dma_alloc_coherent(size_t size);
void dma_free_coherent(void * mem, size_t size);
physical_addr_t virt_to_phys(virtual_addr_t virt);
virtual_addr_t phys_to_virt(physical_addr_t phys);

#ifdef __cplusplus
}
#endif

#endif /* __X64_MMU_H__ */
