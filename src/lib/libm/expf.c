/*
 * libm/expf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float
	one			= 1.0,
	halF[2]		= {0.5,-0.5,},
	huge		= 1.0e+30,
	twom100 	= 7.8886090522e-31,
	o_threshold	= 8.8721679688e+01,
	u_threshold	= -1.0397208405e+02,
	ln2HI[2]   	= { 6.9314575195e-01,
					-6.9314575195e-01,},
	ln2LO[2]   	= { 1.4286067653e-06,
					-1.4286067653e-06,},
	invln2 		=  1.4426950216e+00,
	P1			=  1.6666667163e-01,
	P2			= -2.7777778450e-03,
	P3			=  6.6137559770e-05,
	P4			= -1.6533901999e-06,
	P5			=  4.1381369442e-08;

float expf(float x)
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
EXPORT_SYMBOL(expf);
