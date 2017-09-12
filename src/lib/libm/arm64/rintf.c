float rintf(float x)
{
	__asm__ __volatile__("frintx %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
