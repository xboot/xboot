/*
 * libm/sinhf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float one = 1.0, shuge = 1.0e37;

float sinhf(float x)
{
	float t, w, h;
	s32_t ix, jx;

	GET_FLOAT_WORD(jx,x);
	ix = jx & 0x7fffffff;

	if (ix >= 0x7f800000)
		return x + x;

	h = 0.5;
	if (jx < 0)
		h = -h;

	if (ix < 0x41100000)
	{
		if (ix < 0x39800000)
			if (shuge + x > one)
				return x;
		t = expm1f(fabsf(x));
		if (ix < 0x3f800000)
			return h * ((float) 2.0 * t - t * t / (t + one));
		return h * (t + t / (t + one));
	}

	if (ix < 0x42b17217)
		return h * expf(fabsf(x));

	if (ix <= 0x42b2d4fc)
	{
		w = expf((float) 0.5 * fabsf(x));
		t = h * w;
		return t * w;
	}

	return x * shuge;
}
EXPORT_SYMBOL(sinhf);
