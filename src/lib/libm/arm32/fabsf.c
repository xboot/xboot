#if __ARM32_ARCH__ >= 7
float fabsf(float x)
{
	__asm__ __volatile__("vabs.f32 %0, %1" : "=t"(x) : "t"(x));
	return x;
}
#endif
