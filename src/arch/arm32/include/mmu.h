#ifndef __ARM32_MMU_H__
#define __ARM32_MMU_H__

#include <xboot.h>

#ifdef __cplusplus
extern "C" {
#endif

physical_addr_t virt_to_phys(virtual_addr_t virt);
virtual_addr_t phys_to_virt(physical_addr_t phys);
void * dma_alloc_coherent(size_t size);
void dma_free_coherent(void * mem, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_MMU_H__ */
