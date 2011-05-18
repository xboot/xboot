/*
 * libm/expf.c
 */

#include <math.h>

static const float
	one			= 1.0,
	halF[2]		= {0.5,-0.5,},
	huge		= 1.0e+30,
	twom100 	= 7.8886090522e-31,			/* 2**-100=0x0d800000 */
	o_threshold	= 8.8721679688e+01,			/* 0x42b17180 */
	u_threshold	= -1.0397208405e+02,		/* 0xc2cff1b5 */
	ln2HI[2]   	= { 6.9314575195e-01,		/* 0x3f317200 */
					-6.9314575195e-01,},	/* 0xbf317200 */
	ln2LO[2]   	= { 1.4286067653e-06,		/* 0x35bfbe8e */
					-1.4286067653e-06,},	/* 0xb5bfbe8e */
	invln2 		=  1.4426950216e+00,		/* 0x3fb8aa3b */
	P1			=  1.6666667163e-01,		/* 0x3e2aaaab */
	P2			= -2.7777778450e-03,		/* 0xbb360b61 */
	P3			=  6.6137559770e-05,		/* 0x388ab355 */
	P4			= -1.6533901999e-06,		/* 0xb5ddea0e */
	P5			=  4.1381369442e-08;		/* 0x3331bb4c */

float __ieee754_expf(float x)
{
	float y, hi = 0.0, lo = 0.0, c, t;
	s32_t k = 0, xsb;
	u32_t hx;

	GET_FLOAT_WORD(hx,x);
	xsb = (hx >> 31) & 1;
	hx &= 0x7fffffff;

	if (hx >= 0x42b17218)
	{
		if (hx > 0x7f800000)
			return x + x;
		if (hx == 0x7f800000)
			return (xsb == 0) ? x : 0.0;
		if (x > o_threshold)
			return huge * huge;
		if (x < u_threshold)
			return twom100 * twom100;
	}

	if (hx > 0x3eb17218)
	{
		if (hx < 0x3F851592)
		{
			hi = x - ln2HI[xsb];
			lo = ln2LO[xsb];
			k = 1 - xsb - xsb;
		}
		else
		{
			k = invln2 * x + halF[xsb];
			t = k;
			hi = x - t * ln2HI[0];
			lo = t * ln2LO[0];
		}
		x = hi - lo;
	}
	else if (hx < 0x31800000)
	{
		if (huge + x > one)
			return one + x;
	}
	else
		k = 0;

	t = x * x;
	c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
	if (k == 0)
		return one - ((x * c) / (c - (float) 2.0) - x);
	else
		y = one - ((lo - (x * c) / ((float) 2.0 - c)) - hi);
	if (k >= -125)
	{
		u32_t hy;
		GET_FLOAT_WORD(hy,y);
		SET_FLOAT_WORD(y,hy+(k<<23));
		return y;
	}
	else
	{
		u32_t hy;
		GET_FLOAT_WORD(hy,y);
		SET_FLOAT_WORD(y,hy+((k+100)<<23));
		return y * twom100;
	}
}
