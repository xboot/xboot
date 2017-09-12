#include <math.h>

#define EPS DBL_EPSILON
static const double_t toint = 1/EPS;

static double __round(double x)
{
	union {double f; uint64_t i;} u = {x};
	int e = u.i >> 52 & 0x7ff;
	double_t y;

	if (e >= 0x3ff+52)
		return x;
	if (u.i >> 63)
		x = -x;
	if (e < 0x3ff-1) {
		/* raise inexact if x!=0 */
		FORCE_EVAL(x + toint);
		return 0*u.f;
	}
	y = x + toint - toint - x;
	if (y > 0.5)
		y = y + x - 1;
	else if (y <= -0.5)
		y = y + x + 1;
	else
		y = y + x;
	if (u.i >> 63)
		y = -y;
	return y;
}

extern __typeof(__round) round __attribute__((weak, alias("__round")));
EXPORT_SYMBOL(round);
