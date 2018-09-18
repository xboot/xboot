#ifdef __RISCV64_F__
	.global sqrtf
	.type sqrtf, %function
sqrtf:
	fsqrt.s fa0, fa0
	ret
#endif
