#if __ARM32_ARCH__ >= 7
	.text
	.global fabs
	.type fabs,%function
fabs:
	vabs.f64 d0, d0
	bx lr
#endif
