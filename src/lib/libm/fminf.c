#include <math.h>
#include <xboot/module.h>

static float __fminf(float x, float y)
{
	if (isnan(x))
		return y;
	if (isnan(y))
		return x;
	/* handle signed zeros, see C99 Annex F.9.9.2 */
	if (signbit(x) != signbit(y))
		return signbit(x) ? x : y;
	return x < y ? x : y;
}

extern __typeof(__fminf) fminf __attribute__((weak, alias("__fminf")));
EXPORT_SYMBOL(fminf);
