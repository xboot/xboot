	.text
	.align 4
	.global setjmp
	.type setjmp,%function
setjmp:
	stp x19, x20, [x0,#0]
	stp x21, x22, [x0,#16]
	stp x23, x24, [x0,#32]
	stp x25, x26, [x0,#48]
	stp x27, x28, [x0,#64]
	stp x29, x30, [x0,#80]
	mov x2, sp
	str x2, [x0,#104]
	stp d8, d9, [x0,#112]
	stp d10, d11, [x0,#128]
	stp d12, d13, [x0,#144]
	stp d14, d15, [x0,#160]
	mov x0, #0
	ret

	.text
	.align 4
	.global longjmp
	.type longjmp,%function
longjmp:
	ldp x19, x20, [x0,#0]
	ldp x21, x22, [x0,#16]
	ldp x23, x24, [x0,#32]
	ldp x25, x26, [x0,#48]
	ldp x27, x28, [x0,#64]
	ldp x29, x30, [x0,#80]
	ldr x2, [x0,#104]
	mov sp, x2
	ldp d8 , d9, [x0,#112]
	ldp d10, d11, [x0,#128]
	ldp d12, d13, [x0,#144]
	ldp d14, d15, [x0,#160]

	mov x0, x1
	cbnz x1, 1f
	mov x0, #1
1:	br x30
