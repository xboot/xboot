/*
 * libm/sinh.c
 */

#include <xboot/module.h>
#include <math.h>

static const double one = 1.0, shuge = 1.0e307;

double sinh(double x)
{
	double t, w, h;
	s32_t ix, jx;
	u32_t lx;

	GET_HIGH_WORD(jx,x);
	ix = jx & 0x7fffffff;

	if (ix >= 0x7ff00000)
		return x + x;

	h = 0.5;
	if (jx < 0)
		h = -h;

	if (ix < 0x40360000)
	{
		if (ix < 0x3e300000)
			if (shuge + x > one)
				return x;
		t = expm1(fabs(x));
		if (ix < 0x3ff00000)
			return h * (2.0 * t - t * t / (t + one));
		return h * (t + t / (t + one));
	}

	if (ix < 0x40862E42)
		return h * exp(fabs(x));

	GET_LOW_WORD(lx,x);
	if (ix < 0x408633CE || ((ix == 0x408633ce)
			&& (lx <= (u32_t) 0x8fb9f87d)))
	{
		w = exp(0.5 * fabs(x));
		t = h * w;
		return t * w;
	}

	return x * shuge;
}
EXPORT_SYMBOL(sinh);
