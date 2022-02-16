/*
 * sys-mmu.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <arm32.h>

extern unsigned char __mmu_start[];
extern unsigned char __mmu_end[];

static inline void mmu_ttb_set(uint32_t base)
{
	__asm__ __volatile__("mcr p15, 0, %0, c2, c0, 0" : : "r" (base) : "memory");
}

static inline void mmu_domain_set(uint32_t domain)
{
	__asm__ __volatile__("mcr p15, 0, %0, c3, c0, 0" : : "r" (domain) : "memory");
}

static inline void mmu_inv_tlb(void)
{
	__asm__ __volatile__("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
	__asm__ __volatile__("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));
	dsb();
	isb();
}

static void map_l1_section(uint32_t * ttb, virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type)
{
	physical_size_t i;

	virt >>= 20;
	phys >>= 20;
	size >>= 20;
	type &= 0x3;

	for(i = size; i > 0; i--, virt++, phys++)
		ttb[virt] = (phys << 20) | (1 << 16) | (0x3 << 10) | (0x0 << 5) | (type << 2) | (0x2 << 0);
}

void sys_mmu_init(void)
{
	uint32_t * ttb = (uint32_t *)__mmu_start;

	map_l1_section(ttb, 0x00000000, 0x00000000, SZ_2G, 0);
	map_l1_section(ttb, 0x80000000, 0x80000000, SZ_2G, 0);
	map_l1_section(ttb, 0x40000000, 0x40000000, SZ_128M, 3);
	mmu_ttb_set((uint32_t)(ttb));
	mmu_inv_tlb();
	mmu_domain_set(0x3);
	arm32_mmu_enable();
	arm32_icache_enable();
	arm32_dcache_enable();
}
