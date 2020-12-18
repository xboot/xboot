#if __riscv_flen >= 64
float sqrtf(float x)
{
	__asm__ __volatile__("fsqrt.s %0, %1" : "=f"(x) : "f"(x));
	return x;
}
#endif
