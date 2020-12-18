double fmax(double x, double y)
{
	__asm__ __volatile__("fmaxnm %d0, %d1, %d2" : "=w"(x) : "w"(x), "w"(y));
	return x;
}
