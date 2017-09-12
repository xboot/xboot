double floor(double x)
{
	__asm__ __volatile__("frintm %d0, %d1" : "=w"(x) : "w"(x));
	return x;
}
