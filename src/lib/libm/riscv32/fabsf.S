#ifdef __RISCV32_F__
	.global fabsf
	.type fabsf, %function
fabsf:
	fabs.s fa0, fa0
	ret
#endif
