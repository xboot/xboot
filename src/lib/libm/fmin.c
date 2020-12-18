#include <math.h>
#include <xboot/module.h>

static double __fmin(double x, double y)
{
	if(isnan(x))
		return y;
	if(isnan(y))
		return x;
	if(signbit(x) != signbit(y))
		return signbit(x) ? x : y;
	return x < y ? x : y;
}

extern __typeof(__fmin) fmin __attribute__((weak, alias("__fmin")));
EXPORT_SYMBOL(fmin);
