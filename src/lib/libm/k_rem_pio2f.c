/*
 * libm/k_rem_pio2f.c
 */

#include <math.h>

static const int init_jk[] = { 4, 7, 9 };

static const float PIo2[] = {
	1.5703125000e+00,
	4.5776367188e-04,
	2.5987625122e-05,
	7.5437128544e-08,
	6.0026650317e-11,
	7.3896444519e-13,
	5.3845816694e-15,
	5.6378512969e-18,
	8.3009228831e-20,
	3.2756352257e-22,
	6.3331015649e-25,
};

static const float
	zero = 0.0,
	one = 1.0,
	two8 = 2.5600000000e+02,
	twon8 = 3.9062500000e-03;

int __kernel_rem_pio2f(float *x, float *y, int e0, int nx, int prec,
		const s32_t *ipio2)
{
	s32_t jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
	float z, fw, f[20], fq[20], q[20];

	jk = init_jk[prec];
	jp = jk;

	jx = nx - 1;
	jv = (e0 - 3) / 8;
	if (jv < 0)
		jv = 0;
	q0 = e0 - 8 * (jv + 1);

	j = jv - jx;
	m = jx + jk;
	for (i = 0; i <= m; i++, j++)
		f[i] = (j < 0) ? zero : (float) ipio2[j];

	for (i = 0; i <= jk; i++)
	{
		for (j = 0, fw = 0.0; j <= jx; j++)
			fw += x[j] * f[jx + i - j];
		q[i] = fw;
	}

	jz = jk;
	recompute:
	for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--)
	{
		fw = (float) ((s32_t)(twon8 * z));
		iq[i] = (s32_t)(z - two8 * fw);
		z = q[j - 1] + fw;
	}

	z = scalbnf(z, q0);
	z -= (float) 8.0 * floorf(z * (float) 0.125);
	n = (s32_t) z;
	z -= (float) n;
	ih = 0;
	if (q0 > 0)
	{
		i = (iq[jz - 1] >> (8 - q0));
		n += i;
		iq[jz - 1] -= i << (8 - q0);
		ih = iq[jz - 1] >> (7 - q0);
	}
	else if (q0 == 0)
		ih = iq[jz - 1] >> 7;
	else if (z >= (float) 0.5)
		ih = 2;

	if (ih > 0)
	{
		n += 1;
		carry = 0;
		for (i = 0; i < jz; i++)
		{
			j = iq[i];
			if (carry == 0)
			{
				if (j != 0)
				{
					carry = 1;
					iq[i] = 0x100 - j;
				}
			}
			else
				iq[i] = 0xff - j;
		}
		if (q0 > 0)
		{
			switch (q0)
			{
			case 1:
				iq[jz - 1] &= 0x7f;
				break;
			case 2:
				iq[jz - 1] &= 0x3f;
				break;
			}
		}
		if (ih == 2)
		{
			z = one - z;
			if (carry != 0)
				z -= scalbnf(one, q0);
		}
	}

	if (z == zero)
	{
		j = 0;
		for (i = jz - 1; i >= jk; i--)
			j |= iq[i];
		if (j == 0)
		{
			for (k = 1; iq[jk - k] == 0; k++);

			for (i = jz + 1; i <= jz + k; i++)
			{
				f[jx + i] = (float) ipio2[jv + i];
				for (j = 0, fw = 0.0; j <= jx; j++)
					fw += x[j] * f[jx + i - j];
				q[i] = fw;
			}
			jz += k;
			goto recompute;
		}
	}

	if (z == (float) 0.0)
	{
		jz -= 1;
		q0 -= 8;
		while (iq[jz] == 0)
		{
			jz--;
			q0 -= 8;
		}
	}
	else
	{
		z = scalbnf(z, -q0);
		if (z >= two8)
		{
			fw = (float) ((s32_t)(twon8 * z));
			iq[jz] = (s32_t)(z - two8 * fw);
			jz += 1;
			q0 += 8;
			iq[jz] = (s32_t) fw;
		}
		else
			iq[jz] = (s32_t) z;
	}

	fw = scalbnf(one, q0);
	for (i = jz; i >= 0; i--)
	{
		q[i] = fw * (float) iq[i];
		fw *= twon8;
	}

	for (i = jz; i >= 0; i--)
	{
		for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++)
			fw += PIo2[k] * q[i + k];
		fq[jz - i] = fw;
	}

	switch (prec)
	{
	case 0:
		fw = 0.0;
		for (i = jz; i >= 0; i--)
			fw += fq[i];
		y[0] = (ih == 0) ? fw : -fw;
		break;
	case 1:
	case 2:
		fw = 0.0;
		for (i = jz; i >= 0; i--)
			fw += fq[i];
		fw = *(volatile float *) &fw;
		y[0] = (ih == 0) ? fw : -fw;
		fw = fq[0] - fw;
		for (i = 1; i <= jz; i++)
			fw += fq[i];
		y[1] = (ih == 0) ? fw : -fw;
		break;
	case 3:
		for (i = jz; i > 0; i--)
		{
			fw = fq[i - 1] + fq[i];
			fq[i] += fq[i - 1] - fw;
			fq[i - 1] = fw;
		}
		for (i = jz; i > 1; i--)
		{
			fw = fq[i - 1] + fq[i];
			fq[i] += fq[i - 1] - fw;
			fq[i - 1] = fw;
		}
		for (fw = 0.0, i = jz; i >= 2; i--)
			fw += fq[i];
		if (ih == 0)
		{
			y[0] = fq[0];
			y[1] = fq[1];
			y[2] = fw;
		}
		else
		{
			y[0] = -fq[0];
			y[1] = -fq[1];
			y[2] = -fw;
		}
	}
	return n & 7;
}
