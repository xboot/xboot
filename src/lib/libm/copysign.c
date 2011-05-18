/*
 * libm/copysign.c
 */

#include <math.h>

double copysign(double x, double y)
{
	u32_t hx, hy;

	GET_HIGH_WORD(hx, x);
	GET_HIGH_WORD(hy, y);
	SET_HIGH_WORD(x, (hx & 0x7fffffff) | (hy & 0x80000000));

	return x;
}
