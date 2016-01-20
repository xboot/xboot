/*
 * mmu.c
 */

#include <mmu.h>

void mmu_setup(struct machine_t * mach)
{
}

physical_addr_t virt_to_phys(virtual_addr_t virt)
{
	return (physical_addr_t)virt;
}

virtual_addr_t phys_to_virt(physical_addr_t phys)
{
	return (virtual_addr_t)phys;
}
