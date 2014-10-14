#ifdef __ARM_NEON__
	.text
	.global fabsf
	.type   fabsf,%function
fabsf:
	vabs.f32 s0, s0
	bx lr
#endif
