#if __riscv_flen >= 64
	.global fabs
	.type fabs, %function
	.align 3
fabs:
	fabs.d fa0, fa0
	ret
#endif
