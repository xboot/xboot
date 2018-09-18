#ifdef __RISCV64_D__
	.global sqrt
	.type sqrt, %function
sqrt:
	fsqrt.d fa0, fa0
	ret
#endif
