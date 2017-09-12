double sqrt(double x)
{
	__asm__ __volatile__("fsqrt %d0, %d1" : "=w"(x) : "w"(x));
	return x;
}
