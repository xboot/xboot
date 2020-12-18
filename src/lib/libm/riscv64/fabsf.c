#if __riscv_flen >= 64
float fabsf(float x)
{
	__asm__ __volatile__("fabs.s %0, %1" : "=f"(x) : "f"(x));
	return x;
}
#endif
