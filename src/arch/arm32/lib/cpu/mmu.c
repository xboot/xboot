/*
 * mmu.c
 */

#include <arm32.h>
#include <mmu.h>

/*
 * MMU translation table base address
 */
static u32_t __mmu_ttb[4096] __attribute__((aligned(0x4000)));

static void map_l1_section(virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type)
{
	physical_size_t i;

	virt >>= 20;
	phys >>= 20;
	size >>= 20;
	type &= 0x3;

	for(i = size; i > 0; i--, virt++, phys++)
		__mmu_ttb[virt] = (phys << 20) | (0x3 << 10) | (0x0 << 5) | (type << 2) | (0x2 << 0);
}

void mmu_setup(const struct mmap_t * map)
{
	struct mmap_t * m = (struct mmap_t *)map;

	if(m)
	{
		map_l1_section(0x00000000, 0x00000000, SZ_2G, 0);
		map_l1_section(0x80000000, 0x80000000, SZ_2G, 0);

		while(m->size > 0)
		{
			map_l1_section(m->virt, m->phys, m->size, m->type);
			m++;
		}

		arm32_ttb_set((u32_t)(__mmu_ttb));
		arm32_tlb_invalidate();
		arm32_domain_set(0x3);
		arm32_mmu_enable();
		arm32_icache_enable();
		arm32_dcache_enable();
	}
}
