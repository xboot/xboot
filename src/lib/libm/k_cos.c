/*
 * libm/k_cos.c
 */

#include <math.h>

static const double
	one = 1.00000000000000000000e+00,
	C1 = 4.16666666666666019037e-02,
	C2 = -1.38888888888741095749e-03,
	C3 = 2.48015872894767294178e-05,
	C4 = -2.75573143513906633035e-07,
	C5 = 2.08757232129817482790e-09,
	C6 = -1.13596475577881948265e-11;

double __kernel_cos(double x, double y)
{
	double hz, z, r, w;

	z = x * x;
	r = z * (C1 + z * (C2 + z * (C3 + z * (C4 + z * (C5 + z * C6)))));
	hz = (float) 0.5 * z;
	w = one - hz;
	return w + (((one - w) - hz) + (z * r - x * y));
}
