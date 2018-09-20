#if __riscv_flen >= 32
	.global sqrtf
	.type sqrtf, %function
	.align 4
sqrtf:
	fsqrt.s fa0, fa0
	ret
#endif
