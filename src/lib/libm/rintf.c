#include <math.h>

#define EPS FLT_EPSILON
static const float_t toint = 1/EPS;

static float __rintf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = u.i>>23 & 0xff;
	int s = u.i>>31;
	float_t y;

	if (e >= 0x7f+23)
		return x;
	if (s)
		y = x - toint + toint;
	else
		y = x + toint - toint;
	if (y == 0)
		return s ? -0.0f : 0.0f;
	return y;
}

extern __typeof(__rintf) rintf __attribute__((weak, alias("__rintf")));
EXPORT_SYMBOL(rintf);

