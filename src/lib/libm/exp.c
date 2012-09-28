/*
 * libm/exp.c
 */

#include <xboot/module.h>
#include <math.h>

static const double
	one			= 1.0,
	halF[2]		= {0.5,-0.5,},
	huge		= 1.0e+300,
	twom1000	= 9.33263618503218878990e-302,
	o_threshold	= 7.09782712893383973096e+02,
	u_threshold	= -7.45133219101941108420e+02,
	ln2HI[2]	= { 6.93147180369123816490e-01,
					-6.93147180369123816490e-01,},
	ln2LO[2]	={ 1.90821492927058770002e-10,
					-1.90821492927058770002e-10,},
	invln2		=  1.44269504088896338700e+00,
	P1			=  1.66666666666666019037e-01,
	P2			= -2.77777777770155933842e-03,
	P3			=  6.61375632143793436117e-05,
	P4			= -1.65339022054652515390e-06,
	P5			=  4.13813679705723846039e-08;

double exp(double x)
{
	double y, hi = 0.0, lo = 0.0, c, t;
	s32_t k = 0, xsb;
	u32_t hx;

	GET_HIGH_WORD(hx,x);
	xsb = (hx >> 31) & 1;
	hx &= 0x7fffffff;

	if (hx >= 0x40862E42)
	{
		if (hx >= 0x7ff00000)
		{
			u32_t lx;
			GET_LOW_WORD(lx,x);
			if (((hx & 0xfffff) | lx) != 0)
				return x + x;
			else
				return (xsb == 0) ? x : 0.0;
		}
		if (x > o_threshold)
			return huge * huge;
		if (x < u_threshold)
			return twom1000 * twom1000;
	}

	if (hx > 0x3fd62e42)
	{
		if (hx < 0x3FF0A2B2)
		{
			hi = x - ln2HI[xsb];
			lo = ln2LO[xsb];
			k = 1 - xsb - xsb;
		}
		else
		{
			k = (int) (invln2 * x + halF[xsb]);
			t = k;
			hi = x - t * ln2HI[0];
			lo = t * ln2LO[0];
		}
		x = hi - lo;
	}
	else if (hx < 0x3e300000)
	{
		if (huge + x > one)
			return one + x;
	}
	else
		k = 0;

	t = x * x;
	c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
	if (k == 0)
		return one - ((x * c) / (c - 2.0) - x);
	else
		y = one - ((lo - (x * c) / (2.0 - c)) - hi);
	if (k >= -1021)
	{
		u32_t hy;
		GET_HIGH_WORD(hy,y);
		SET_HIGH_WORD(y,hy+(k<<20));
		return y;
	}
	else
	{
		u32_t hy;
		GET_HIGH_WORD(hy,y);
		SET_HIGH_WORD(y,hy+((k+1000)<<20));
		return y * twom1000;
	}
}
EXPORT_SYMBOL(exp);
