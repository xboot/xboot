#ifndef __ARM64_MMU_H__
#define __ARM64_MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void * dma_alloc_coherent(size_t size);
void dma_free_coherent(void * mem, size_t size);
physical_addr_t virt_to_phys(virtual_addr_t virt);
virtual_addr_t phys_to_virt(physical_addr_t phys);

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_MMU_H__ */
