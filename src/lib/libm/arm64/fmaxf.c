float fmaxf(float x, float y)
{
	__asm__ __volatile__("fmaxnm %s0, %s1, %s2" : "=w"(x) : "w"(x), "w"(y));
	return x;
}
