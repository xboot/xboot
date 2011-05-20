/*
 * libm/k_cosf.c
 */

#include <math.h>

static const double
	one = 1.0,
	C0 = -0x1ffffffd0c5e81.0p-54,
	C1 = 0x155553e1053a42.0p-57,
	C2 = -0x16c087e80f1e27.0p-62,
	C3 = 0x199342e0ee5069.0p-68;

float __kernel_cosdf(double x)
{
	double r, w, z;

	z = x * x;
	w = z * z;
	r = C2 + z * C3;
	return ((one + z * C0) + w * C1) + (w * z) * r;
}
