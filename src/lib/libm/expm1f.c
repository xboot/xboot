/*
 * libm/expm1f.c
 */

#include <math.h>

static const float
	one = 1.0,
	huge = 1.0e+30,
	tiny = 1.0e-30,
	o_threshold = 8.8721679688e+01,
	ln2_hi = 6.9313812256e-01,
	ln2_lo = 9.0580006145e-06,
	invln2 = 1.4426950216e+00,
	Q1 = -3.3333335072e-02,
	Q2 = 1.5873016091e-03,
	Q3 = -7.9365076090e-05,
	Q4 = 4.0082177293e-06,
	Q5 = -2.0109921195e-07;

float expm1f(float x)
{
	float y, hi, lo, c=0, t, e, hxs, hfx, r1;
	s32_t k, xsb;
	u32_t hx;

	GET_FLOAT_WORD(hx,x);
	xsb = hx & 0x80000000;
	if (xsb == 0)
		y = x;
	else
		y = -x;
	hx &= 0x7fffffff;

	if (hx >= 0x4195b844)
	{
		if (hx >= 0x42b17218)
		{
			if (hx > 0x7f800000)
				return x + x;
			if (hx == 0x7f800000)
				return (xsb == 0) ? x : -1.0;
			if (x > o_threshold)
				return huge * huge;
		}
		if (xsb != 0)
		{
			if (x + tiny < (float) 0.0)
				return tiny - one;
		}
	}

	if (hx > 0x3eb17218)
	{
		if (hx < 0x3F851592)
		{
			if (xsb == 0)
			{
				hi = x - ln2_hi;
				lo = ln2_lo;
				k = 1;
			}
			else
			{
				hi = x + ln2_hi;
				lo = -ln2_lo;
				k = -1;
			}
		}
		else
		{
			k = invln2 * x + ((xsb == 0) ? (float) 0.5 : (float) -0.5);
			t = k;
			hi = x - t * ln2_hi;
			lo = t * ln2_lo;
		}
		x = hi - lo;
		c = (hi - x) - lo;
	}
	else if (hx < 0x33000000)
	{
		t = huge + x;
		return x - (t - (huge + x));
	}
	else
		k = 0;

	hfx = (float) 0.5 * x;
	hxs = x * hfx;
	r1 = one + hxs * (Q1 + hxs * (Q2 + hxs * (Q3 + hxs * (Q4 + hxs * Q5))));
	t = (float) 3.0 - r1 * hfx;
	e = hxs * ((r1 - t) / ((float) 6.0 - x * t));
	if (k == 0)
		return x - (x * e - hxs);
	else
	{
		e = (x * (e - c) - c);
		e -= hxs;
		if (k == -1)
			return (float) 0.5 * (x - e) - (float) 0.5;
		if (k == 1)
		{
			if (x < (float) -0.25)
				return -(float) 2.0 * (e - (x + (float) 0.5));
			else
				return one + (float) 2.0 * (x - e);
		}
		if (k <= -2 || k > 56)
		{
			s32_t i;
			y = one - (e - x);
			GET_FLOAT_WORD(i,y);
			SET_FLOAT_WORD(y,i+(k<<23));
			return y - one;
		}
		t = one;
		if (k < 23)
		{
			s32_t i;
			SET_FLOAT_WORD(t,0x3f800000 - (0x1000000>>k));
			y = t - (e - x);
			GET_FLOAT_WORD(i,y);
			SET_FLOAT_WORD(y,i+(k<<23));
		}
		else
		{
			s32_t i;
			SET_FLOAT_WORD(t,((0x7f-k)<<23));
			y = x - (e + t);
			y += one;
			GET_FLOAT_WORD(i,y);
			SET_FLOAT_WORD(y,i+(k<<23));
		}
	}
	return y;
}
