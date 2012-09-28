/*
 * libm/log10f.c
 */

#include <xboot/module.h>
#include <math.h>

static const float
	two25 = 3.3554432000e+07,
	ivln10 = 4.3429449201e-01,
	log10_2hi = 3.0102920532e-01,
	log10_2lo = 7.9034151668e-07;

static const float zero = 0.0;

float log10f(float x)
{
	float y, z;
	s32_t i, k, hx;

	GET_FLOAT_WORD(hx,x);

	k = 0;
	if (hx < 0x00800000)
	{
		if ((hx & 0x7fffffff) == 0)
			return -two25 / zero;
		if (hx < 0)
			return (x - x) / zero;
		k -= 25;
		x *= two25;
		GET_FLOAT_WORD(hx,x);
	}
	if (hx >= 0x7f800000)
		return x + x;
	k += (hx >> 23) - 127;
	i = ((u32_t) k & 0x80000000) >> 31;
	hx = (hx & 0x007fffff) | ((0x7f - i) << 23);
	y = (float) (k + i);
	SET_FLOAT_WORD(x,hx);
	z = y * log10_2lo + ivln10 * logf(x);
	return z + y * log10_2hi;
}
EXPORT_SYMBOL(log10f);
