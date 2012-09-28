/*
 * libm/modf.c
 */

#include <xboot/module.h>
#include <math.h>

static const double one = 1.0;

double modf(double x, double *iptr)
{
	s32_t i0, i1, j0;
	u32_t i;

	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
	if (j0 < 20)
	{
		if (j0 < 0)
		{
			INSERT_WORDS(*iptr,i0&0x80000000,0);
			return x;
		}
		else
		{
			i = (0x000fffff) >> j0;
			if (((i0 & i) | i1) == 0)
			{
				u32_t high;
				*iptr = x;
				GET_HIGH_WORD(high,x);
				INSERT_WORDS(x,high&0x80000000,0);
				return x;
			}
			else
			{
				INSERT_WORDS(*iptr,i0&(~i),0);
				return x - *iptr;
			}
		}
	}
	else if (j0 > 51)
	{
		u32_t high;
		*iptr = x * one;
		GET_HIGH_WORD(high,x);
		INSERT_WORDS(x,high&0x80000000,0);
		return x;
	}
	else
	{
		i = ((u32_t)(0xffffffff)) >> (j0 - 20);
		if ((i1 & i) == 0)
		{
			u32_t high;
			*iptr = x;
			GET_HIGH_WORD(high,x);
			INSERT_WORDS(x,high&0x80000000,0);
			return x;
		}
		else
		{
			INSERT_WORDS(*iptr,i0,i1&(~i));
			return x - *iptr;
		}
	}
}
EXPORT_SYMBOL(modf);
