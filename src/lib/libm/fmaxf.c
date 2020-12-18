#include <math.h>
#include <xboot/module.h>

static float __fmaxf(float x, float y)
{
	if(isnan(x))
		return y;
	if(isnan(y))
		return x;
	if(signbit(x) != signbit(y))
		return signbit(x) ? y : x;
	return x < y ? y : x;
}

extern __typeof(__fmaxf) fmaxf __attribute__((weak, alias("__fmaxf")));
EXPORT_SYMBOL(fmaxf);
