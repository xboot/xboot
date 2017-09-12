#include <math.h>

#define EPS DBL_EPSILON
static const double_t toint = 1/EPS;

static double __rint(double x)
{
	union {double f; uint64_t i;} u = {x};
	int e = u.i>>52 & 0x7ff;
	int s = u.i>>63;
	double_t y;

	if (e >= 0x3ff+52)
		return x;
	if (s)
		y = x - toint + toint;
	else
		y = x + toint - toint;
	if (y == 0)
		return s ? -0.0 : 0;
	return y;
}

extern __typeof(__rint) rint __attribute__((weak, alias("__rint")));
EXPORT_SYMBOL(rint);
