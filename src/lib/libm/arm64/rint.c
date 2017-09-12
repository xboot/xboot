double rint(double x)
{
	__asm__ __volatile__("frintx %d0, %d1" : "=w"(x) : "w"(x));
	return x;
}
