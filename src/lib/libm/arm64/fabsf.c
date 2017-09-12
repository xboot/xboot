float fabsf(float x)
{
	__asm__ __volatile__("fabs %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
