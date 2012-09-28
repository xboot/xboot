/*
 * libm/coshf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float one = 1.0, half = 0.5, huge = 1.0e30;

float coshf(float x)
{
	float t, w;
	s32_t ix;

	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;

	if (ix >= 0x7f800000)
		return x * x;

	if (ix < 0x3eb17218)
	{
		t = expm1f(fabsf(x));
		w = one + t;
		if (ix < 0x39800000)
			return one;
		return one + (t * t) / (w + w);
	}

	if (ix < 0x41100000)
	{
		t = expf(fabsf(x));
		return half * t + half / t;
	}

	if (ix < 0x42b17217)
		return half * expf(fabsf(x));

	if (ix <= 0x42b2d4fc)
	{
		w = expf(half * fabsf(x));
		t = half * w;
		return t * w;
	}

	return huge * huge;
}
EXPORT_SYMBOL(coshf);
