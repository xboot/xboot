float ceilf(float x)
{
	__asm__ __volatile__("frintp %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
