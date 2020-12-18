#if __riscv_flen >= 64
double sqrt(double x)
{
	__asm__ __volatile__("fsqrt.d %0, %1" : "=f"(x) : "f"(x));
	return x;
}
#endif
