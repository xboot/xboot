#if __ARM_ARCH__ >= 7
	.text
	.global sqrtf
	.type sqrtf,%function
sqrtf:
	vsqrt.f32 s0, s0
	bx lr
#endif
