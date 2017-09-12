float truncf(float x)
{
	__asm__ __volatile__("frintz %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
