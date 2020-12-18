double sqrt(double x)
{
	__asm__ __volatile__("sqrtsd %1, %0" : "=x"(x) : "x"(x));
	return x;
}
