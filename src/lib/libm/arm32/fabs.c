#if __ARM32_ARCH__ >= 7
double fabs(double x)
{
	__asm__ __volatile__("vabs.f64 %P0, %P1" : "=w"(x) : "w"(x));
	return x;
}
#endif
