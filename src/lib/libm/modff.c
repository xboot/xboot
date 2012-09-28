/*
 * libm/modff.c
 */

#include <xboot/module.h>
#include <math.h>

static const float one = 1.0;

float modff(float x, float *iptr)
{
	s32_t i0, j0;
	u32_t i;

	GET_FLOAT_WORD(i0,x);
	j0 = ((i0 >> 23) & 0xff) - 0x7f;
	if (j0 < 23)
	{
		if (j0 < 0)
		{
			SET_FLOAT_WORD(*iptr,i0&0x80000000);
			return x;
		}
		else
		{
			i = (0x007fffff) >> j0;
			if ((i0 & i) == 0)
			{
				u32_t ix;
				*iptr = x;
				GET_FLOAT_WORD(ix,x);
				SET_FLOAT_WORD(x,ix&0x80000000);
				return x;
			}
			else
			{
				SET_FLOAT_WORD(*iptr,i0&(~i));
				return x - *iptr;
			}
		}
	}
	else
	{
		u32_t ix;
		*iptr = x * one;
		GET_FLOAT_WORD(ix,x);
		SET_FLOAT_WORD(x,ix&0x80000000);
		return x;
	}
}
EXPORT_SYMBOL(modff);
