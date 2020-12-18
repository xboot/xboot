#if __riscv_flen >= 64
float fminf(float x, float y)
{
	__asm__ __volatile__("fmin.s %0, %1, %2" : "=f"(x) : "f"(x), "f"(y));
	return x;
}
#endif
