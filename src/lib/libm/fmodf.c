/*
 * libm/fmodf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float one = 1.0, Zero[] = {0.0, -0.0,};

float fmodf(float x, float y)
{
	s32_t n, hx, hy, hz, ix, iy, sx, i;

	GET_FLOAT_WORD(hx,x);
	GET_FLOAT_WORD(hy,y);
	sx = hx & 0x80000000;
	hx ^= sx;
	hy &= 0x7fffffff;

	if (hy == 0 || (hx >= 0x7f800000) || (hy > 0x7f800000))
		return (x * y) / (x * y);
	if (hx < hy)
		return x;
	if (hx == hy)
		return Zero[(u32_t) sx >> 31];

	if (hx < 0x00800000)
	{
		for (ix = -126, i = (hx << 8); i > 0; i <<= 1)
			ix -= 1;
	}
	else
		ix = (hx >> 23) - 127;

	if (hy < 0x00800000)
	{
		for (iy = -126, i = (hy << 8); i >= 0; i <<= 1)
			iy -= 1;
	}
	else
		iy = (hy >> 23) - 127;

	if (ix >= -126)
		hx = 0x00800000 | (0x007fffff & hx);
	else
	{
		n = -126 - ix;
		hx = hx << n;
	}
	if (iy >= -126)
		hy = 0x00800000 | (0x007fffff & hy);
	else
	{
		n = -126 - iy;
		hy = hy << n;
	}

	n = ix - iy;
	while (n--)
	{
		hz = hx - hy;
		if (hz < 0)
		{
			hx = hx + hx;
		}
		else
		{
			if (hz == 0)
				return Zero[(u32_t) sx >> 31];
			hx = hz + hz;
		}
	}
	hz = hx - hy;
	if (hz >= 0)
	{
		hx = hz;
	}

	if (hx == 0)
		return Zero[(u32_t) sx >> 31];
	while (hx < 0x00800000)
	{
		hx = hx + hx;
		iy -= 1;
	}
	if (iy >= -126)
	{
		hx = ((hx - 0x00800000) | ((iy + 127) << 23));
		SET_FLOAT_WORD(x,hx|sx);
	}
	else
	{
		n = -126 - iy;
		hx >>= n;
		SET_FLOAT_WORD(x,hx|sx);
		x *= one;
	}
	return x;
}
EXPORT_SYMBOL(fmodf);
