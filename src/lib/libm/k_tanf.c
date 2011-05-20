/*
 * libm/k_tanf.c
 */

#include <math.h>

static const double T[] = {
	0x15554d3418c99f.0p-54,
	0x1112fd38999f72.0p-55,
	0x1b54c91d865afe.0p-57,
	0x191df3908c33ce.0p-58,
	0x185dadfcecf44e.0p-61,
	0x1362b9bf971bcd.0p-59,
};

float __kernel_tandf(double x, int iy)
{
	double z, r, w, s, t, u;

	z = x * x;

	r = T[4] + z * T[5];
	t = T[2] + z * T[3];
	w = z * z;
	s = z * x;
	u = T[0] + z * T[1];
	r = (x + s * u) + (s * w) * (t + w * r);
	if (iy == 1)
		return r;
	else
		return -1.0 / r;
}
