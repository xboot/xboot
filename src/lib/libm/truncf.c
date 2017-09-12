#include <math.h>

static float __truncf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f + 9;
	uint32_t m;

	if (e >= 23 + 9)
		return x;
	if (e < 9)
		e = 1;
	m = -1U >> e;
	if ((u.i & m) == 0)
		return x;
	FORCE_EVAL(x + 0x1p120f);
	u.i &= ~m;
	return u.f;
}

extern __typeof(__truncf) truncf __attribute__((weak, alias("__truncf")));
EXPORT_SYMBOL(truncf);
