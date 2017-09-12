float roundf(float x)
{
	__asm__ __volatile__("frinta %s0, %s1" : "=w"(x) : "w"(x));
	return x;
}
