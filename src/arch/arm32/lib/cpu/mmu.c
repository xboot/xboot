/*
 * mmu.c
 */

#include <cp15.h>
#include <cache.h>
#include <mmu.h>

/*
 * Hardware page table definitions.
 *
 * + Level 1 descriptor (PMD)
 *   - common
 */
#define PMD_TYPE_MASK		(3 << 0)
#define PMD_TYPE_FAULT		(0 << 0)
#define PMD_TYPE_TABLE		(1 << 0)
#define PMD_TYPE_SECT		(2 << 0)
#define PMD_BIT4		(1 << 4)
#define PMD_DOMAIN(x)		((x) << 5)
#define PMD_PROTECTION		(1 << 9)	/* v5 */
/*
 *   - section
 */
#define PMD_SECT_BUFFERABLE	(1 << 2)
#define PMD_SECT_CACHEABLE	(1 << 3)
#define PMD_SECT_XN		(1 << 4)	/* v6 */
#define PMD_SECT_AP_WRITE	(1 << 10)
#define PMD_SECT_AP_READ	(1 << 11)
#define PMD_SECT_TEX(x)		((x) << 12)	/* v5 */
#define PMD_SECT_APX		(1 << 15)	/* v6 */
#define PMD_SECT_S		(1 << 16)	/* v6 */
#define PMD_SECT_nG		(1 << 17)	/* v6 */
#define PMD_SECT_SUPER		(1 << 18)	/* v6 */

#define PMD_SECT_UNCACHED	(0)
#define PMD_SECT_BUFFERED	(PMD_SECT_BUFFERABLE)
#define PMD_SECT_WT		(PMD_SECT_CACHEABLE)
#define PMD_SECT_WB		(PMD_SECT_CACHEABLE | PMD_SECT_BUFFERABLE)
#define PMD_SECT_MINICACHE	(PMD_SECT_TEX(1) | PMD_SECT_CACHEABLE)
#define PMD_SECT_WBWA		(PMD_SECT_TEX(1) | PMD_SECT_CACHEABLE | PMD_SECT_BUFFERABLE)
#define PMD_SECT_NONSHARED_DEV	(PMD_SECT_TEX(2))

/*
 *   - coarse table (not used)
 */

/*
 * + Level 2 descriptor (PTE)
 *   - common
 */
#define PTE_TYPE_MASK		(3 << 0)
#define PTE_TYPE_FAULT		(0 << 0)
#define PTE_TYPE_LARGE		(1 << 0)
#define PTE_TYPE_SMALL		(2 << 0)
#define PTE_TYPE_EXT		(3 << 0)	/* v5 */
#define PTE_BUFFERABLE		(1 << 2)
#define PTE_CACHEABLE		(1 << 3)

/*
 *   - extended small page/tiny page
 */
#define PTE_EXT_XN		(1 << 0)	/* v6 */
#define PTE_EXT_AP_MASK		(3 << 4)
#define PTE_EXT_AP0		(1 << 4)
#define PTE_EXT_AP1		(2 << 4)
#define PTE_EXT_AP_UNO_SRO	(0 << 4)
#define PTE_EXT_AP_UNO_SRW	(PTE_EXT_AP0)
#define PTE_EXT_AP_URO_SRW	(PTE_EXT_AP1)
#define PTE_EXT_AP_URW_SRW	(PTE_EXT_AP1|PTE_EXT_AP0)
#define PTE_EXT_TEX(x)		((x) << 6)	/* v5 */
#define PTE_EXT_APX		(1 << 9)	/* v6 */
#define PTE_EXT_COHERENT	(1 << 9)	/* XScale3 */
#define PTE_EXT_SHARED		(1 << 10)	/* v6 */
#define PTE_EXT_NG		(1 << 11)	/* v6 */

/*
 *   - small page
 */
#define PTE_SMALL_AP_MASK	(0xff << 4)
#define PTE_SMALL_AP_UNO_SRO	(0x00 << 4)
#define PTE_SMALL_AP_UNO_SRW	(0x55 << 4)
#define PTE_SMALL_AP_URO_SRW	(0xaa << 4)
#define PTE_SMALL_AP_URW_SRW	(0xff << 4)

static u32_t * ttb;

static void create_sections(unsigned long addr, int size_m, unsigned int flags)
{
	int i;

	addr >>= 20;

	for (i = size_m; i > 0; i--, addr++)
		ttb[addr] = (addr << 20) | flags;
}

static void map_cachable(unsigned long start, unsigned long size)
{
	start &= ~(SZ_1M - 1);
	size = (size + (SZ_1M - 1)) & ~(SZ_1M - 1);

	create_sections(start, size >> 20, PMD_SECT_AP_WRITE |
			PMD_SECT_AP_READ | PMD_TYPE_SECT | PMD_SECT_WB);
}

static inline void flush_icache(void)
{
	asm volatile("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));
}

//mmu_early_enable(0x70000000, 0x0c000000 + 0x04000000, 0x7F000000);
void mmu_early_enable(u32_t membase, u32_t memsize, u32_t _ttb)
{
	__mmu_cache_invalidate();

	ttb = (u32_t *)_ttb;

	ttb_set(ttb);

	domain_set(0x3);

	create_sections(0, 4096, PMD_SECT_AP_WRITE |
			PMD_SECT_AP_READ | PMD_TYPE_SECT);

	map_cachable(membase, memsize);

	__mmu_cache_on();

	__mmu_cache_flush();

	flush_icache();

	icache_enable();
	dcache_enable();
	branch_enable();
}

physical_addr_t virt_to_phys(virtual_addr_t virt)
{
	return (physical_addr_t)virt;
}

virtual_addr_t phys_to_virt(physical_addr_t phys)
{
	return (virtual_addr_t)phys;
}

void * dma_alloc_coherent(size_t size)
{
	return NULL;
}

void dma_free_coherent(void * mem, size_t size)
{

}
