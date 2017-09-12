float floorf(float x)
{
	__asm__ __volatile__("frintm %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
