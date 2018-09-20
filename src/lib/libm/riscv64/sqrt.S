#if __riscv_flen >= 64
	.global sqrt
	.type sqrt, %function
	.align 4
sqrt:
	fsqrt.d fa0, fa0
	ret
#endif
