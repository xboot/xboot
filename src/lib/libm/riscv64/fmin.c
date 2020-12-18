#if __riscv_flen >= 64
double fmin(double x, double y)
{
	__asm__ __volatile__("fmin.d %0, %1, %2" : "=f"(x) : "f"(x), "f"(y));
	return x;
}
#endif
