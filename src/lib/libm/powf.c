/*
 * libm/powf.c
 */

#include <xboot/module.h>
#include <math.h>

static const float
	bp[] = { 1.0, 1.5, },
	dp_h[] = { 0.0, 5.84960938e-01, },
	dp_l[] = { 0.0, 1.56322085e-06, },
	zero = 0.0,
	one = 1.0,
	two = 2.0,
	two24 = 16777216.0,
	huge = 1.0e30,
	tiny = 1.0e-30,
	L1 = 6.0000002384e-01,
	L2 = 4.2857143283e-01,
	L3 = 3.3333334327e-01,
	L4 = 2.7272811532e-01,
	L5 = 2.3066075146e-01,
	L6 = 2.0697501302e-01,
	P1 = 1.6666667163e-01,
	P2 = -2.7777778450e-03,
	P3 = 6.6137559770e-05,
	P4 = -1.6533901999e-06,
	P5 = 4.1381369442e-08,
	lg2 = 6.9314718246e-01,
	lg2_h = 6.93145752e-01,
	lg2_l = 1.42860654e-06,
	ovt = 4.2995665694e-08,
	cp = 9.6179670095e-01,
	cp_h = 9.6179199219e-01,
	cp_l = 4.7017383622e-06,
	ivln2 = 1.4426950216e+00,
	ivln2_h = 1.4426879883e+00,
	ivln2_l = 7.0526075433e-06;

float powf(float x, float y)
{
	float z, ax, z_h, z_l, p_h, p_l;
	float y1, t1, t2, r, s, sn, t, u, v, w;
	s32_t i, j, k, yisint, n;
	s32_t hx, hy, ix, iy, is;

	GET_FLOAT_WORD(hx,x);
	GET_FLOAT_WORD(hy,y);
	ix = hx & 0x7fffffff;
	iy = hy & 0x7fffffff;

	if (iy == 0)
		return one;

	if (ix > 0x7f800000 || iy > 0x7f800000)
		return x + y;

	yisint = 0;
	if (hx < 0)
	{
		if (iy >= 0x4b800000)
			yisint = 2;
		else if (iy >= 0x3f800000)
		{
			k = (iy >> 23) - 0x7f;
			j = iy >> (23 - k);
			if ((j << (23 - k)) == iy)
				yisint = 2 - (j & 1);
		}
	}

	if (iy == 0x7f800000)
	{
		if (ix == 0x3f800000)
			return y - y;
		else if (ix > 0x3f800000)
			return (hy >= 0) ? y : zero;
		else
			return (hy < 0) ? -y : zero;
	}
	if (iy == 0x3f800000)
	{
		if (hy < 0)
			return one / x;
		else
			return x;
	}
	if (hy == 0x40000000)
		return x * x;
	if (hy == 0x3f000000)
	{
		if (hx >= 0)
			return sqrtf(x);
	}

	ax = fabsf(x);

	if (ix == 0x7f800000 || ix == 0 || ix == 0x3f800000)
	{
		z = ax;
		if (hy < 0)
			z = one / z;
		if (hx < 0)
		{
			if (((ix - 0x3f800000) | yisint) == 0)
			{
				z = (z - z) / (z - z);
			}
			else if (yisint == 1)
				z = -z;
		}
		return z;
	}

	n = ((u32_t) hx >> 31) - 1;

	if ((n | yisint) == 0)
		return (x - x) / (x - x);

	sn = one;
	if ((n | (yisint - 1)) == 0)
		sn = -one;

	if (iy > 0x4d000000)
	{
		if (ix < 0x3f7ffff8)
			return (hy < 0) ? sn * huge * huge : sn * tiny * tiny;
		if (ix > 0x3f800007)
			return (hy > 0) ? sn * huge * huge : sn * tiny * tiny;

		t = ax - 1;
		w = (t * t) * ((float) 0.5 - t * ((float) 0.333333333333 - t
				* (float) 0.25));
		u = ivln2_h * t;
		v = t * ivln2_l - w * ivln2;
		t1 = u + v;
		GET_FLOAT_WORD(is,t1);
		SET_FLOAT_WORD(t1,is&0xfffff000);
		t2 = v - (t1 - u);
	}
	else
	{
		float s2, s_h, s_l, t_h, t_l;
		n = 0;

		if (ix < 0x00800000)
		{
			ax *= two24;
			n -= 24;
			GET_FLOAT_WORD(ix,ax);
		}
		n += ((ix) >> 23) - 0x7f;
		j = ix & 0x007fffff;

		ix = j | 0x3f800000;
		if (j <= 0x1cc471)
			k = 0;
		else if (j < 0x5db3d7)
			k = 1;
		else
		{
			k = 0;
			n += 1;
			ix -= 0x00800000;
		}
		SET_FLOAT_WORD(ax,ix);

		u = ax - bp[k];
		v = one / (ax + bp[k]);
		s = u * v;
		s_h = s;
		GET_FLOAT_WORD(is,s_h);
		SET_FLOAT_WORD(s_h,is&0xfffff000);

		is = ((ix >> 1) & 0xfffff000) | 0x20000000;
		SET_FLOAT_WORD(t_h,is+0x00400000+(k<<21));
		t_l = ax - (t_h - bp[k]);
		s_l = v * ((u - s_h * t_h) - s_h * t_l);

		s2 = s * s;
		r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2
				* (L5 + s2 * L6)))));
		r += s_l * (s_h + s);
		s2 = s_h * s_h;
		t_h = (float) 3.0 + s2 + r;
		GET_FLOAT_WORD(is,t_h);
		SET_FLOAT_WORD(t_h,is&0xfffff000);
		t_l = r - ((t_h - (float) 3.0) - s2);

		u = s_h * t_h;
		v = s_l * t_h + t_l * s;

		p_h = u + v;
		GET_FLOAT_WORD(is,p_h);
		SET_FLOAT_WORD(p_h,is&0xfffff000);
		p_l = v - (p_h - u);
		z_h = cp_h * p_h;
		z_l = cp_l * p_h + p_l * cp + dp_l[k];

		t = (float) n;
		t1 = (((z_h + z_l) + dp_h[k]) + t);
		GET_FLOAT_WORD(is,t1);
		SET_FLOAT_WORD(t1,is&0xfffff000);
		t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
	}

	GET_FLOAT_WORD(is,y);
	SET_FLOAT_WORD(y1,is&0xfffff000);
	p_l = (y - y1) * t1 + y * t2;
	p_h = y1 * t1;
	z = p_l + p_h;
	GET_FLOAT_WORD(j,z);
	if (j > 0x43000000)
		return sn * huge * huge;
	else if (j == 0x43000000)
	{
		if (p_l + ovt > z - p_h)
			return sn * huge * huge;
	}
	else if ((j & 0x7fffffff) > 0x43160000)
		return sn * tiny * tiny;
	else if (j == 0xc3160000)
	{
		if (p_l <= z - p_h)
			return sn * tiny * tiny;
	}

	i = j & 0x7fffffff;
	k = (i >> 23) - 0x7f;
	n = 0;
	if (i > 0x3f000000)
	{
		n = j + (0x00800000 >> (k + 1));
		k = ((n & 0x7fffffff) >> 23) - 0x7f;
		SET_FLOAT_WORD(t,n&~(0x007fffff>>k));
		n = ((n & 0x007fffff) | 0x00800000) >> (23 - k);
		if (j < 0)
			n = -n;
		p_h -= t;
	}
	t = p_l + p_h;
	GET_FLOAT_WORD(is,t);
	SET_FLOAT_WORD(t,is&0xffff8000);
	u = t * lg2_h;
	v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
	z = u + v;
	w = v - (z - u);
	t = z * z;
	t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
	r = (z * t1) / (t1 - two) - (w + z * w);
	z = one - (r - z);
	GET_FLOAT_WORD(j,z);
	j += (n << 23);
	if ((j >> 23) <= 0)
		z = scalbnf(z, n);
	else
		SET_FLOAT_WORD(z,j);
	return sn * z;
}
EXPORT_SYMBOL(powf);
