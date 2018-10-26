/*
 *  ----------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6    |    7    |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18  |   0x1c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  | fc_mxcsr|fc_x87_cw|        R12        |         R13        |        R14        |  *
 *  ----------------------------------------------------------------------------------  *
 *  ----------------------------------------------------------------------------------  *
 *  |    8    |    9    |   10    |   11    |    12    |    13   |    14   |    15   |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x20  |   0x24  |   0x28  |  0x2c   |   0x30   |   0x34  |   0x38  |   0x3c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        R15        |        RBX        |         RBP        |        RIP        |  *
 *  ----------------------------------------------------------------------------------  *
 */

	.text
	.globl make_context
	.type make_context, @function
	.align 4
make_context:
	/* first arg of make_fcontext() == top of context-stack */
	movq %rdi, %rax

	/* shift address in RAX to lower 16 byte boundary */
	andq $-16, %rax

	/* reserve space for context-data on context-stack */
	/* on context-function entry: (RSP -0x8) % 16 == 0 */
	leaq -0x40(%rax), %rax

	/* third arg of make_fcontext() == address of context-function */
	/* stored in RBX */
	movq %rdx, 0x28(%rax)

	/* save MMX control- and status-word */
	stmxcsr (%rax)
	/* save x87 control-word */
	fnstcw 0x4(%rax)

	/* compute abs address of label trampoline */
	leaq trampoline(%rip), %rcx
	/* save address of trampoline as return-address for context-function */
	/* will be entered after calling jump_fcontext() first time */
	movq %rcx, 0x38(%rax)

	/* compute abs address of label finish */
	leaq finish(%rip), %rcx
	/* save address of finish as return-address for context-function */
	/* will be entered after context-function returns */
	movq %rcx, 0x30(%rax)

	ret /* return pointer to context-data */

trampoline:
	/* store return address on stack */
	/* fix stack alignment */
	push %rbp
	/* jump to context-function */
	jmp *%rbx

finish:
	/* exit code is zero */
	xorq %rdi, %rdi
	/* exit application */
	call exit
	hlt

/*
 *  ----------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6    |    7    |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18  |   0x1c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  | fc_mxcsr|fc_x87_cw|        R12        |         R13        |        R14        |  *
 *  ----------------------------------------------------------------------------------  *
 *  ----------------------------------------------------------------------------------  *
 *  |    8    |    9    |   10    |   11    |    12    |    13   |    14   |    15   |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x20  |   0x24  |   0x28  |  0x2c   |   0x30   |   0x34  |   0x38  |   0x3c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        R15        |        RBX        |         RBP        |        RIP        |  *
 *  ----------------------------------------------------------------------------------  *
 */

	.text
	.globl jump_context
	.type jump_context, @function
	.align 4
jump_context:
	leaq  -0x38(%rsp), %rsp /* prepare stack */

	stmxcsr (%rsp)     /* save MMX control- and status-word */
	fnstcw 0x4(%rsp)  /* save x87 control-word */

	movq %r12, 0x8(%rsp)  /* save R12 */
	movq %r13, 0x10(%rsp)  /* save R13 */
	movq %r14, 0x18(%rsp)  /* save R14 */
	movq %r15, 0x20(%rsp)  /* save R15 */
	movq %rbx, 0x28(%rsp)  /* save RBX */
	movq %rbp, 0x30(%rsp)  /* save RBP */

	/* store RSP (pointing to context-data) in RAX */
	movq  %rsp, %rax

	/* restore RSP (pointing to context-data) from RDI */
	movq  %rdi, %rsp

	movq  0x38(%rsp), %r8  /* restore return-address */

	ldmxcsr (%rsp)     /* restore MMX control- and status-word */
	fldcw 0x4(%rsp)  /* restore x87 control-word */

	movq 0x8(%rsp), %r12  /* restore R12 */
	movq 0x10(%rsp), %r13  /* restore R13 */
	movq 0x18(%rsp), %r14  /* restore R14 */
	movq 0x20(%rsp), %r15  /* restore R15 */
	movq 0x28(%rsp), %rbx  /* restore RBX */
	movq 0x30(%rsp), %rbp  /* restore RBP */

	leaq 0x40(%rsp), %rsp /* prepare stack */

	/* return transfer_t from jump */
	/* RAX == fctx, RDX == data */
	movq %rsi, %rdx
	/* pass transfer_t as first arg in context function */
	/* RDI == fctx, RSI == data */
	movq %rax, %rdi

	/* indirect jump to context */
	jmp *%r8
