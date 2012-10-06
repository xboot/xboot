/*
 * libm/scalbn.c
 */

#include <math.h>

static const double
	two54 = 1.80143985094819840000e+16,
	twom54 = 5.55111512312578270212e-17,
	huge = 1.0e+300, tiny = 1.0e-300;

double scalbn(double x, int n)
{
	s32_t k, hx, lx;

	EXTRACT_WORDS(hx,lx,x);
	k = (hx & 0x7ff00000) >> 20;
	if (k == 0)
	{
		if ((lx | (hx & 0x7fffffff)) == 0)
			return x;
		x *= two54;
		GET_HIGH_WORD(hx,x);
		k = ((hx & 0x7ff00000) >> 20) - 54;
		if (n < -50000)
			return tiny * x;
	}
	if (k == 0x7ff)
		return x + x;
	k = k + n;
	if (k > 0x7fe)
		return huge * copysign(huge, x);
	if (k > 0)
	{
		SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));
		return x;
	}
	if (k <= -54)
	{
		if (n > 50000)
			return huge * copysign(huge, x);
		else
			return tiny * copysign(tiny, x);
	}
	k += 54;
	SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));

	return x * twom54;
}

double ldexp(double x, int n)
{
	return scalbn(x, n);
}
