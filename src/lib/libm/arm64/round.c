double round(double x)
{
	__asm__ __volatile__("frinta %d0, %d1" : "=w"(x) : "w"(x));
	return x;
}
