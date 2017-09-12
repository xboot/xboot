#if __ARM32_ARCH__ >= 7
double sqrt(double x)
{
	__asm__ __volatile__("vsqrt.f64 %P0, %P1" : "=w"(x) : "w"(x));
	return x;
}
#endif
