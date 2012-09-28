/*
 * libm/frexpf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float two25 = 3.3554432000e+07;

float frexpf(float x, int *eptr)
{
	s32_t hx, ix;

	GET_FLOAT_WORD(hx,x);
	ix = 0x7fffffff & hx;
	*eptr = 0;

	if (ix >= 0x7f800000 || (ix == 0))
		return x;

	if (ix < 0x00800000)
	{
		x *= two25;
		GET_FLOAT_WORD(hx,x);
		ix = hx & 0x7fffffff;
		*eptr = -25;
	}

	*eptr += (ix >> 23) - 126;
	hx = (hx & 0x807fffff) | 0x3f000000;
	SET_FLOAT_WORD(x,hx);

	return x;
}
EXPORT_SYMBOL(frexpf);
