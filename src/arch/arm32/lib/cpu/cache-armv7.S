#include <linkage.h>

.section .text.v7_mmu_cache_on
ENTRY(v7_mmu_cache_on)
		stmfd	sp!, {r11, lr}
		mov	r12, lr
#ifdef CONFIG_MMU
		mrc	p15, 0, r11, c0, c1, 4	@ read ID_MMFR0
		tst	r11, #0xf		@ VMSA
		mov	r0, #0
		mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
		tst	r11, #0xf		@ VMSA
		mcrne	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs
#endif
		mrc	p15, 0, r0, c1, c0, 0	@ read control reg
		orr	r0, r0, #0x5000		@ I-cache enable, RR cache replacement
		orr	r0, r0, #0x003c		@ write buffer
#ifdef CONFIG_MMU
#ifdef __BIG_ENDIAN
		orr	r0, r0, #1 << 25	@ big-endian page tables
#endif
		orrne	r0, r0, #1		@ MMU enabled
		movne	r1, #-1
		mcrne	p15, 0, r1, c3, c0, 0	@ load domain access control
#endif
		mcr	p15, 0, r0, c7, c5, 4	@ ISB
		mcr	p15, 0, r0, c1, c0, 0	@ load control register
		mrc	p15, 0, r0, c1, c0, 0	@ and read it back
		mov	r0, #0
		mcr	p15, 0, r0, c7, c5, 4	@ ISB
		ldmfd	sp!, {r11, pc}
ENDPROC(v7_mmu_cache_on)

.section .text.v7_mmu_cache_off
ENTRY(v7_mmu_cache_off)
		/* although 'r12' is an eabi scratch register which does
		   not need to be restored, save it to ensure an 8-byte
	           stack alignment */
		stmfd	sp!, {r4-r12, lr}
		mrc	p15, 0, r0, c1, c0
#ifdef CONFIG_MMU
		bic	r0, r0, #0x000d
#else
		bic	r0, r0, #0x000c
#endif
		mcr	p15, 0, r0, c1, c0	@ turn MMU and cache off
		bl	v7_mmu_cache_flush
		mov	r0, #0
#ifdef CONFIG_MMU
		mcr	p15, 0, r0, c8, c7, 0	@ invalidate whole TLB
#endif
		mcr	p15, 0, r0, c7, c5, 6	@ invalidate BTC
		mcr	p15, 0, r0, c7, c10, 4	@ DSB
		mcr	p15, 0, r0, c7, c5, 4	@ ISB
		ldmfd	sp!, {r4-r12, pc}
ENDPROC(v7_mmu_cache_off)

.section .text.v7_mmu_cache_flush_invalidate
ENTRY(v7_mmu_cache_invalidate)
		mov	r0, #1
		b	__v7_mmu_cache_flush_invalidate
ENDPROC(v7_mmu_cache_invalidate)

ENTRY(v7_mmu_cache_flush)
		mov	r0, #0
		b	__v7_mmu_cache_flush_invalidate
ENDPROC(v7_mmu_cache_flush)

ENTRY(__v7_mmu_cache_flush_invalidate)
		mrc	p15, 0, r12, c0, c1, 5	@ read ID_MMFR1
		tst	r12, #0xf << 16		@ hierarchical cache (ARMv7)
		mov	r12, #0
		beq	hierarchical
		mcr	p15, 0, r12, c7, c14, 0	@ clean+invalidate D
		b	iflush
hierarchical:
		stmfd	sp!, {r4-r11}
		mov	r8, r0
		mcr	p15, 0, r12, c7, c10, 5	@ DMB
		mrc	p15, 1, r0, c0, c0, 1	@ read clidr
		ands	r3, r0, #0x7000000	@ extract loc from clidr
		mov	r3, r3, lsr #23		@ left align loc bit field
		beq	finished		@ if loc is 0, then no need to clean
		mov	r12, #0			@ start clean at cache level 0
loop1:
		add	r2, r12, r12, lsr #1	@ work out 3x current cache level
		mov	r1, r0, lsr r2		@ extract cache type bits from clidr
		and	r1, r1, #7		@ mask of the bits for current cache only
		cmp	r1, #2			@ see what cache we have at this level
		blt	skip			@ skip if no cache, or just i-cache
		mcr	p15, 2, r12, c0, c0, 0	@ select current cache level in cssr
		mcr	p15, 0, r12, c7, c5, 4	@ isb to sych the new cssr&csidr
		mrc	p15, 1, r1, c0, c0, 0	@ read the new csidr
		and	r2, r1, #7		@ extract the length of the cache lines
		add	r2, r2, #4		@ add 4 (line length offset)
		ldr	r4, =0x3ff
		ands	r4, r4, r1, lsr #3	@ find maximum number on the way size
		clz	r5, r4			@ find bit position of way size increment
		ldr	r7, =0x7fff
		ands	r7, r7, r1, lsr #13	@ extract max number of the index size
loop2:
		mov	r9, r4			@ create working copy of max way size
loop3:
ARM(		orr	r11, r12, r9, lsl r5	) @ factor way and cache number into r11
ARM(		orr	r11, r11, r7, lsl r2	) @ factor index number into r11
THUMB(		lsl	r6, r9, r5		)
THUMB(		orr	r11, r12, r6		) @ factor way and cache number into r11
THUMB(		lsl	r6, r7, r2		)
THUMB(		orr	r11, r11, r6		) @ factor index number into r11
		cmp	r8, #0
THUMB(		ite	eq			)
		mcreq	p15, 0, r11, c7, c14, 2	@ clean & invalidate by set/way
		mcrne	p15, 0, r11, c7, c6, 2	@ invalidate by set/way
		subs	r9, r9, #1		@ decrement the way
		bge	loop3
		subs	r7, r7, #1		@ decrement the index
		bge	loop2
skip:
		add	r12, r12, #2		@ increment cache number
		cmp	r3, r12
		bgt	loop1
finished:
		ldmfd	sp!, {r4-r11}
		mov	r12, #0			@ switch back to cache level 0
		mcr	p15, 2, r12, c0, c0, 0	@ select current cache level in cssr
iflush:
		mcr	p15, 0, r12, c7, c10, 4	@ DSB
		mcr	p15, 0, r12, c7, c5, 0	@ invalidate I+BTB
		mcr	p15, 0, r12, c7, c10, 4	@ DSB
		mcr	p15, 0, r12, c7, c5, 4	@ ISB
		mov	pc, lr
ENDPROC(__v7_mmu_cache_flush_invalidate)

/*
 * cache_line_size - get the cache line size from the CSIDR register
 * (available on ARMv7+). It assumes that the CSSR register was configured
 * to access the L1 data cache CSIDR.
 */
	.macro	dcache_line_size, reg, tmp
	mrc	p15, 1, \tmp, c0, c0, 0		@ read CSIDR
	and	\tmp, \tmp, #7			@ cache line size encoding
	mov	\reg, #16			@ size offset
	mov	\reg, \reg, lsl \tmp		@ actual cache line size
	.endm

/*
 *	v7_dma_inv_range(start,end)
 *
 *	Invalidate the data cache within the specified region; we will
 *	be performing a DMA operation in this region and we want to
 *	purge old data in the cache.
 *
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v7_dma_inv_range
ENTRY(v7_dma_inv_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	tst	r0, r3
	bic	r0, r0, r3
	mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line

	tst	r1, r3
	bic	r1, r1, r3
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line
1:
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(v7_dma_inv_range)

/*
 *	v7_dma_clean_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v7_dma_clean_range
ENTRY(v7_dma_clean_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c10, 1		@ clean D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(v7_dma_clean_range)

/*
 *	v7_dma_flush_range(start,end)
 *	- start   - virtual start address of region
 *	- end     - virtual end address of region
 */
.section .text.v7_dma_flush_range
ENTRY(v7_dma_flush_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(v7_dma_flush_range)
