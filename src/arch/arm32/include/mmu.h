#ifndef __ARM32_MMU_H__
#define __ARM32_MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

physical_addr_t virt_to_phys(virtual_addr_t virt);
virtual_addr_t phys_to_virt(physical_addr_t phys);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_MMU_H__ */
