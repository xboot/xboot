#include <linkage.h>

#define HARVARD_CACHE
#define CACHE_LINE_SIZE		32
#define D_CACHE_LINE_SIZE	32

.section .text.v6_mmu_cache_on
ENTRY(v6_mmu_cache_on)
		mov	r12, lr
#ifdef CONFIG_MMU
		mov	r0, #0
		mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
		mcr	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs
		mrc	p15, 0, r0, c1, c0, 0	@ read control reg
		orr	r0, r0, #0x5000		@ I-cache enable, RR cache replacement
		orr	r0, r0, #0x0030
#ifdef __BIG_ENDIAN
		orr	r0, r0, #1 << 25	@ big-endian page tables
#endif
		bl	__common_mmu_cache_on
		mov	r0, #0
		mcr	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs
#endif
		mov	pc, r12
ENDPROC(v6_mmu_cache_on)

__common_mmu_cache_on:
		orr	r0, r0, #0x000d		@ Write buffer, mmu
		b	1f
		.align	5			@ cache line aligned
1:		mcr	p15, 0, r0, c1, c0, 0	@ load control register
		mrc	p15, 0, r0, c1, c0, 0	@ and read it back to
		sub	pc, lr, r0, lsr #32	@ properly flush pipeline


.section .text.v6_mmu_cache_off
ENTRY(v6_mmu_cache_off)
#ifdef CONFIG_MMU
		mrc	p15, 0, r0, c1, c0
		bic	r0, r0, #0x000d
		mcr	p15, 0, r0, c1, c0	@ turn MMU and cache off
		mov	r0, #0
		mcr	p15, 0, r0, c7, c7	@ invalidate whole cache v4
		mcr	p15, 0, r0, c8, c7	@ invalidate whole TLB v4
#endif
		mov	pc, lr

.section .text.v6_mmu_cache_flush
ENTRY(v6_mmu_cache_flush)
		mov	r1, #0
		mcr	p15, 0, r1, c7, c14, 0	@ clean+invalidate D
		mcr	p15, 0, r1, c7, c5, 0	@ invalidate I+BTB
		mcr	p15, 0, r1, c7, c15, 0	@ clean+invalidate unified
		mcr	p15, 0, r1, c7, c10, 4	@ drain WB
		mov	pc, lr
ENDPROC(v6_mmu_cache_flush)

/*
 *	v6_dma_inv_range(start,end)
 *
 *	Invalidate the data cache within the specified region; we will
 *	be performing a DMA operation in this region and we want to
 *	purge old data in the cache.
 *
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v6_dma_inv_range
ENTRY(v6_dma_inv_range)
	tst	r0, #D_CACHE_LINE_SIZE - 1
	bic	r0, r0, #D_CACHE_LINE_SIZE - 1
#ifdef HARVARD_CACHE
	mcrne	p15, 0, r0, c7, c10, 1		@ clean D line
#else
	mcrne	p15, 0, r0, c7, c11, 1		@ clean unified line
#endif
	tst	r1, #D_CACHE_LINE_SIZE - 1
	bic	r1, r1, #D_CACHE_LINE_SIZE - 1
#ifdef HARVARD_CACHE
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D line
#else
	mcrne	p15, 0, r1, c7, c15, 1		@ clean & invalidate unified line
#endif
1:
#ifdef HARVARD_CACHE
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D line
#else
	mcr	p15, 0, r0, c7, c7, 1		@ invalidate unified line
#endif
	add	r0, r0, #D_CACHE_LINE_SIZE
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_dma_inv_range)

/*
 *	v6_dma_clean_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v6_dma_clean_range
ENTRY(v6_dma_clean_range)
	bic	r0, r0, #D_CACHE_LINE_SIZE - 1
1:
#ifdef HARVARD_CACHE
	mcr	p15, 0, r0, c7, c10, 1		@ clean D line
#else
	mcr	p15, 0, r0, c7, c11, 1		@ clean unified line
#endif
	add	r0, r0, #D_CACHE_LINE_SIZE
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_dma_clean_range)

/*
 *	v6_dma_flush_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v6_dma_flush_range
ENTRY(v6_dma_flush_range)
	bic	r0, r0, #D_CACHE_LINE_SIZE - 1
1:
#ifdef HARVARD_CACHE
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D line
#else
	mcr	p15, 0, r0, c7, c15, 1		@ clean & invalidate line
#endif
	add	r0, r0, #D_CACHE_LINE_SIZE
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_dma_flush_range)
