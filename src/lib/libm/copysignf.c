/*
 * libm/copysignf.c
 */

#include <math.h>

float copysignf(float x, float y)
{
	u32_t ix, iy;

	GET_FLOAT_WORD(ix, x);
	GET_FLOAT_WORD(iy, y);
	SET_FLOAT_WORD(x, (ix & 0x7fffffff) | (iy & 0x80000000));

	return x;
}
