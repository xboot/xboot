/*
 * libm/ceilf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float huge = 1.0e30;

float ceilf(float x)
{
	s32_t i0, j0;
	u32_t i;

	GET_FLOAT_WORD(i0,x);
	j0 = ((i0 >> 23) & 0xff) - 0x7f;

	if (j0 < 23)
	{
		if (j0 < 0)
		{
			if (huge + x > (float) 0.0)
			{
				if (i0 < 0)
				{
					i0 = 0x80000000;
				}
				else if (i0 != 0)
				{
					i0 = 0x3f800000;
				}
			}
		}
		else
		{
			i = (0x007fffff) >> j0;
			if ((i0 & i) == 0)
				return x;
			if (huge + x > (float) 0.0)
			{
				if (i0 > 0)
					i0 += (0x00800000) >> j0;
				i0 &= (~i);
			}
		}
	}
	else
	{
		if (j0 == 0x80)
			return x + x;
		else
			return x;
	}

	SET_FLOAT_WORD(x,i0);
	return x;
}
EXPORT_SYMBOL(ceilf);
