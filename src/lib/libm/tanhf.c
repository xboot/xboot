/*
 * libm/tanhf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float one = 1.0, two = 2.0, tiny = 1.0e-30, huge = 1.0e30;

float tanhf(float x)
{
	float t, z;
	s32_t jx, ix;

	GET_FLOAT_WORD(jx,x);
	ix = jx & 0x7fffffff;

	if (ix >= 0x7f800000)
	{
		if (jx >= 0)
			return one / x + one;
		else
			return one / x - one;
	}

	if (ix < 0x41100000)
	{
		if (ix < 0x39800000)
		{
			if (huge + x > one)
				return x;
		}
		if (ix >= 0x3f800000)
		{
			t = expm1f(two * fabsf(x));
			z = one - two / (t + two);
		}
		else
		{
			t = expm1f(-two * fabsf(x));
			z = -t / (t + two);
		}
	}
	else
	{
		z = one - tiny;
	}
	return (jx >= 0) ? z : -z;
}
EXPORT_SYMBOL(tanhf);
