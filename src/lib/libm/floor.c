/*
 * libm/floor.c
 */

#include <xboot/module.h>
#include <math.h>

static const double huge = 1.0e300;

double floor(double x)
{
	s32_t i0, i1, j0;
	u32_t i, j;

	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;

	if (j0 < 20)
	{
		if (j0 < 0)
		{
			if (huge + x > 0.0)
			{
				if (i0 >= 0)
				{
					i0 = i1 = 0;
				}
				else if (((i0 & 0x7fffffff) | i1) != 0)
				{
					i0 = 0xbff00000;
					i1 = 0;
				}
			}
		}
		else
		{
			i = (0x000fffff) >> j0;
			if (((i0 & i) | i1) == 0)
				return x;
			if (huge + x > 0.0)
			{
				if (i0 < 0)
					i0 += (0x00100000) >> j0;
				i0 &= (~i);
				i1 = 0;
			}
		}
	}
	else if (j0 > 51)
	{
		if (j0 == 0x400)
			return x + x;
		else
			return x;
	}
	else
	{
		i = ((u32_t)(0xffffffff)) >> (j0 - 20);
		if ((i1 & i) == 0)
			return x;
		if (huge + x > 0.0)
		{
			if (i0 < 0)
			{
				if (j0 == 20)
					i0 += 1;
				else
				{
					j = i1 + (1 << (52 - j0));
					if (j < i1)
						i0 += 1;
					i1 = j;
				}
			}
			i1 &= (~i);
		}
	}

	INSERT_WORDS(x,i0,i1);
	return x;
}
EXPORT_SYMBOL(floor);
