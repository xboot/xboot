/*
 * libm/k_sinf.c
 */

#include <math.h>

static const double
	S1 = -0x15555554cbac77.0p-55,
	S2 = 0x111110896efbb2.0p-59,
	S3 = -0x1a00f9e2cae774.0p-65,
	S4 = 0x16cd878c3b46a7.0p-71;

float __kernel_sindf(double x)
{
	double r, s, w, z;

	z = x * x;
	w = z * z;
	r = S3 + z * S4;
	s = z * x;
	return (x + s * (S1 + z * S2)) + s * w * r;
}
