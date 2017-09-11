#include <xconfigs.h>
#include <linkage.h>

#if __ARM32_ARCH__ == 4

ENTRY(v4_cache_inv_range)
	tst	r0, #32 - 1
	bic	r0, r0, #32 - 1
	mcrne p15, 0, r0, c7, c10, 1	@ clean D entry
	tst	r1, #32 - 1
	mcrne p15, 0, r1, c7, c10, 1	@ clean D entry
1:	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr
ENDPROC(v4_cache_inv_range)

ENTRY(v4_cache_clean_range)
	bic	r0, r0, #32 - 1
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr
ENDPROC(v4_cache_clean_range)

ENTRY(v4_cache_flush_range)
	bic	r0, r0, #32 - 1
1:	mcr	p15, 0, r0, c7, c14, 1		@ clean + invalidate D entry
	add	r0, r0, #32
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr
ENDPROC(v4_cache_flush_range)

#endif
