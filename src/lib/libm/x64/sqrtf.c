float sqrtf(float x)
{
	__asm__ __volatile__("sqrtss %1, %0" : "=x"(x) : "x"(x));
	return x;
}
