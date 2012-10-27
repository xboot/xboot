/*
 * __aeabi_idivmod.S
 */

	.globl	__aeabi_uidivmod
	.align	4

__aeabi_uidivmod:
	stmfd	sp!, {r0, r1, ip, lr}
	bl	__aeabi_uidiv
	ldmfd	sp!, {r1, r2, ip, lr}
	mul	r3, r0, r2
	sub	r1, r1, r3
	mov	pc, lr
