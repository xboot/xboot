/*
 * libm/sin.c
 */

#include <xboot/module.h>
#include <math.h>

double sin(double x)
{
	double y[2], z = 0.0;
	s32_t n, ix;

	GET_HIGH_WORD(ix,x);

	ix &= 0x7fffffff;
	if (ix <= 0x3fe921fb)
	{
		if (ix < 0x3e400000)
		{
			if ((int) x == 0)
				return x;
		}
		return __kernel_sin(x, z, 0);
	}
	else if (ix >= 0x7ff00000)
		return x - x;
	else
	{
		n = __ieee754_rem_pio2(x, y);
		switch (n & 3)
		{
		case 0:
			return __kernel_sin(y[0], y[1], 1);
		case 1:
			return __kernel_cos(y[0], y[1]);
		case 2:
			return -__kernel_sin(y[0], y[1], 1);
		default:
			return -__kernel_cos(y[0], y[1]);
		}
	}
}
EXPORT_SYMBOL(sin);
