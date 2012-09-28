/*
 * libm/cosh.c
 */

#include <xboot/module.h>
#include <math.h>

static const double one = 1.0, half = 0.5, huge = 1.0e300;

double cosh(double x)
{
	double t, w;
	s32_t ix;
	u32_t lx;

	GET_HIGH_WORD(ix,x);
	ix &= 0x7fffffff;

	if (ix >= 0x7ff00000)
		return x * x;

	if (ix < 0x3fd62e43)
	{
		t = expm1(fabs(x));
		w = one + t;
		if (ix < 0x3c800000)
			return w;
		return one + (t * t) / (w + w);
	}

	if (ix < 0x40360000)
	{
		t = exp(fabs(x));
		return half * t + half / t;
	}

	if (ix < 0x40862E42)
		return half * exp(fabs(x));

	GET_LOW_WORD(lx,x);
	if (ix < 0x408633CE || ((ix == 0x408633ce)
			&& (lx <= (u32_t) 0x8fb9f87d)))
	{
		w = exp(half * fabs(x));
		t = half * w;
		return t * w;
	}

	return huge * huge;
}
EXPORT_SYMBOL(cosh);
