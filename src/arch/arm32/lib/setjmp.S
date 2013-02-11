/*
 * setjmp/longjmp for the ARM architecture
 *
 * Pure ARM version
 *
 * The jmp_buf is assumed to contain the following, in order:
 * 		r4
 * 		r5
 * 		r6
 * 		r7
 * 		r8
 * 		r9
 * 		r10
 * 		fp
 * 		sp
 * 		lr
 */

	.text
	.balign	4
	.globl	setjmp
	.type	setjmp, #function
setjmp:
	stmia	r0, {r4, r5, r6, r7, r8, r9, r10, fp, sp, lr}
	mov		r0, #0
	bx		lr
	.size	setjmp,.-setjmp

	.text
	.balign	4
	.globl	longjmp
	.type	longjmp, #function
longjmp:
	ldmia	r0, {r4, r5, r6, r7, r8, r9, r10, fp, sp, lr}
	movs	r0, r1
	moveq	r0, #1
	bx		lr
	.size longjmp,.-longjmp
