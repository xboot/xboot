#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct mmap_t {
	const char * name;
	virtual_addr_t virt;
	physical_addr_t phys;
	physical_size_t size;
	int flag;
};

void memory_map(struct mmap_t * map);
virtual_addr_t phys_to_virt(physical_addr_t phys);
physical_addr_t virt_to_phys(virtual_addr_t virt);

#ifdef __cplusplus
}
#endif

#endif /* __MEMORY_H__ */
