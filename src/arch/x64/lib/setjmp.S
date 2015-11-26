	.text
	.align 4
	.global setjmp
	.type setjmp,%function
setjmp:
	mov %rbx,(%rdi)
	mov %rbp,8(%rdi)
	mov %r12,16(%rdi)
	mov %r13,24(%rdi)
	mov %r14,32(%rdi)
	mov %r15,40(%rdi)
	lea 8(%rsp),%rdx
	mov %rdx,48(%rdi)
	mov (%rsp),%rdx
	mov %rdx,56(%rdi)
	xor %rax,%rax
	ret

	.text
	.align 4
	.global longjmp
	.type longjmp,%function
longjmp:
	mov %rsi,%rax
	test %rax,%rax
	jnz 1f
	inc %rax
1:
	mov (%rdi),%rbx
	mov 8(%rdi),%rbp
	mov 16(%rdi),%r12
	mov 24(%rdi),%r13
	mov 32(%rdi),%r14
	mov 40(%rdi),%r15
	mov 48(%rdi),%rdx
	mov %rdx,%rsp
	mov 56(%rdi),%rdx
	jmp *%rdx
