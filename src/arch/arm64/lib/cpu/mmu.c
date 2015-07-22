/*
 * mmu.c
 */

#include <mmu.h>

#define PAGE_SIZE			(4096)
#define PAGE_SHIFT			(12)
#define PAGE_ALIGN(x)		(((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

void mmu_setup(struct machine_t * mach)
{
}

void * dma_alloc_coherent(size_t size)
{
	return memalign(PAGE_SIZE, size);
}

void dma_free_coherent(void * mem, size_t size)
{
	free(mem);
}

physical_addr_t virt_to_phys(virtual_addr_t virt)
{
	return (physical_addr_t)virt;
}

virtual_addr_t phys_to_virt(physical_addr_t phys)
{
	return (virtual_addr_t)phys;
}
