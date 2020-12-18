#if __riscv_flen >= 64
float fmaxf(float x, float y)
{
	__asm__ __volatile__("fmax.s %0, %1, %2" : "=f"(x) : "f"(x), "f"(y));
	return x;
}
#endif
