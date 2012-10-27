/*
 * __aeabi_idivmod.S
 */

	.globl	__aeabi_idivmod
	.align	4

__aeabi_idivmod:
	stmfd	sp!, {r0, r1, ip, lr}
	bl	__aeabi_idiv
	ldmfd	sp!, {r1, r2, ip, lr}
	mul	r3, r0, r2
	sub	r1, r1, r3
	mov	pc, lr
