#include <xconfigs.h>
#include <linkage.h>

#if __ARM32_ARCH__ == 7

.macro dcache_line_size, reg, tmp
	mrc	p15, 1, \tmp, c0, c0, 0		@ read CSIDR
	and	\tmp, \tmp, #7				@ cache line size encoding
	mov	\reg, #16					@ size offset
	mov \reg, \reg, lsl \tmp		@ actual cache line size
.endm

ENTRY(v7_cache_inv_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	tst	r0, r3
	bic	r0, r0, r3
	mcrne p15, 0, r0, c7, c14, 1	@ clean & invalidate D / U line

	tst	r1, r3
	bic	r1, r1, r3
	mcrne p15, 0, r1, c7, c14, 1	@ clean & invalidate D / U line
1:
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(v7_cache_inv_range)

ENTRY(v7_cache_clean_range)
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
ENDPROC(v7_cache_clean_range)

ENTRY(v7_cache_flush_range)
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
ENDPROC(v7_cache_flush_range)

#endif
