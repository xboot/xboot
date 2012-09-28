/*
 * libm/acosf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float
	one = 1.0000000000e+00,
	pi = 3.1415925026e+00,
	pio2_hi = 1.5707962513e+00,
	pio2_lo = 7.5497894159e-08,
	pS0 = 1.6666667163e-01,
	pS1 = -3.2556581497e-01,
	pS2 = 2.0121252537e-01,
	pS3 = -4.0055535734e-02,
	pS4 = 7.9153501429e-04,
	pS5 = 3.4793309169e-05,
	qS1 = -2.4033949375e+00,
	qS2 = 2.0209457874e+00,
	qS3 = -6.8828397989e-01,
	qS4 = 7.7038154006e-02;

float acosf(float x)
{
	float z, p, q, r, w, s, c, df;
	s32_t hx, ix;

	GET_FLOAT_WORD(hx,x);
	ix = hx & 0x7fffffff;
	if (ix == 0x3f800000)
	{
		if (hx > 0)
			return 0.0;
		else
			return pi + (float) 2.0 * pio2_lo;
	}
	else if (ix > 0x3f800000)
	{
		return (x - x) / (x - x);
	}
	if (ix < 0x3f000000)
	{
		if (ix <= 0x23000000)
			return pio2_hi + pio2_lo;
		z = x * x;
		p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
		q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
		r = p / q;
		return pio2_hi - (x - (pio2_lo - x * r));
	}
	else if (hx < 0)
	{
		z = (one + x) * (float) 0.5;
		p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
		q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
		s = sqrtf(z);
		r = p / q;
		w = r * s - pio2_lo;
		return pi - (float) 2.0 * (s + w);
	}
	else
	{
		s32_t idf;
		z = (one - x) * (float) 0.5;
		s = sqrtf(z);
		df = s;
		GET_FLOAT_WORD(idf,df);
		SET_FLOAT_WORD(df,idf&0xfffff000);
		c = (z - df * df) / (s + df);
		p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
		q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
		r = p / q;
		w = r * s + c;
		return (float) 2.0 * (df + w);
	}
}
EXPORT_SYMBOL(acosf);
