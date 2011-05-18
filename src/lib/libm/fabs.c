/*
 * libm/fabs.c
 */

#include <math.h>

double fabs(double x)
{
	u32_t high;

	GET_HIGH_WORD(high, x);
	SET_HIGH_WORD(x, high & 0x7fffffff);
	return x;
}
