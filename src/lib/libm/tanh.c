/*
 * libm/tanh.c
 */

#include <xboot/module.h>
#include <math.h>

static const double one = 1.0, two = 2.0, tiny = 1.0e-300;

double tanh(double x)
{
	double t, z;
	s32_t jx, ix;

	GET_HIGH_WORD(jx,x);
	ix = jx & 0x7fffffff;

	if (ix >= 0x7ff00000)
	{
		if (jx >= 0)
			return one / x + one;
		else
			return one / x - one;
	}

	if (ix < 0x40360000)
	{
		if (ix < 0x3c800000)
			return x * (one + x);
		if (ix >= 0x3ff00000)
		{
			t = expm1(two * fabs(x));
			z = one - two / (t + two);
		}
		else
		{
			t = expm1(-two * fabs(x));
			z = -t / (t + two);
		}
	}
	else
	{
		z = one - tiny;
	}
	return (jx >= 0) ? z : -z;
}
EXPORT_SYMBOL(tanh);
