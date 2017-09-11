#include <xconfigs.h>
#include <linkage.h>

#if __ARM32_ARCH__ == 6

ENTRY(v6_cache_inv_range)
	tst	r0, #32 - 1
	bic	r0, r0, #32 - 1
	mcrne p15, 0, r0, c7, c10, 1	@ clean D line
	tst	r1, #32 - 1
	bic	r1, r1, #32 - 1
	mcrne p15, 0, r1, c7, c14, 1	@ clean & invalidate D line
1:
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D line
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_cache_inv_range)

ENTRY(v6_cache_clean_range)
	bic	r0, r0, #32 - 1
1:
	mcr	p15, 0, r0, c7, c10, 1		@ clean D line
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_cache_clean_range)

ENTRY(v6_cache_flush_range)
	bic	r0, r0, #32 - 1
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D line
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer
	mov	pc, lr
ENDPROC(v6_cache_flush_range)

#endif
