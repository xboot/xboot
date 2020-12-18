double fabs(double x)
{
	double t;
	__asm__ __volatile__("pcmpeqd %0, %0" : "=x"(t));
	__asm__ __volatile__("psrlq   $1, %0" : "+x"(t));
	__asm__ __volatile__("andps   %1, %0" : "+x"(x) : "x"(t));
	return x;
}
