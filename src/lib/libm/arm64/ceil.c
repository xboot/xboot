double ceil(double x)
{
	__asm__ __volatile__("frintp %d0, %d1" : "=w"(x) : "w"(x));
	return x;
}
