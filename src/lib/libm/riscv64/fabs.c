#if __riscv_flen >= 64
double fabs(double x)
{
	__asm__ __volatile__("fabs.d %0, %1" : "=f"(x) : "f"(x));
	return x;
}
#endif
