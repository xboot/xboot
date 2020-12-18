#include <math.h>
#include <xboot/module.h>

static double __fmax(double x, double y)
{
	if(isnan(x))
		return y;
	if(isnan(y))
		return x;
	if(signbit(x) != signbit(y))
		return signbit(x) ? y : x;
	return x < y ? y : x;
}

extern __typeof(__fmax) fmax __attribute__((weak, alias("__fmax")));
EXPORT_SYMBOL(fmax);
