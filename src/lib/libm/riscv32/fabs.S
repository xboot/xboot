#ifdef __RISCV32_D__
	.global fabs
	.type fabs, %function
fabs:
	fabs.d fa0, fa0
	ret
#endif
