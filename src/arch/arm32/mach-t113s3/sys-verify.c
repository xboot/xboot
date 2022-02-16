/*
 * sys-verify.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>

struct ecdsa256_uint128_t {
	uint64_t m_low;
	uint64_t m_high;
};

struct ecdsa256_point_t {
	uint64_t x[4];
	uint64_t y[4];
};

static void vli_clear(uint64_t * vli)
{
	int i;

	for(i = 0; i < 4; i++)
		vli[i] = 0;
}

static int vli_iszero(uint64_t * vli)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		if(vli[i])
			return 0;
	}
	return 1;
}

static uint64_t vli_testbit(uint64_t * vli, unsigned int bit)
{
	return (vli[bit >> 6] & ((uint64_t)1 << (bit & 0x3f)));
}

static unsigned int vli_numdigits(uint64_t * vli)
{
    int i;

    for(i = 4 - 1; i >= 0 && vli[i] == 0; --i);
    return (i + 1);
}

static unsigned int vli_numbits(uint64_t * vli)
{
	uint64_t digit;
	unsigned int i;
	unsigned int n;

	n = vli_numdigits(vli);
	if(n == 0)
		return 0;
	digit = vli[n - 1];
	for(i = 0; digit; i++)
		digit >>= 1;
	return (((n - 1) << 6) + i);
}

static void vli_set(uint64_t * dst, uint64_t * src)
{
	int i;

	for(i = 0; i < 4; i++)
		dst[i] = src[i];
}

static int vli_cmp(uint64_t * left, uint64_t * right)
{
	int i;

	for(i = 4 - 1; i >= 0; --i)
	{
		if(left[i] > right[i])
			return 1;
		else if(left[i] < right[i])
			return -1;
	}
	return 0;
}

static uint64_t vli_lshift(uint64_t * result, uint64_t * in, unsigned int shift)
{
	uint64_t carry;
	uint64_t temp;
	int i;

	carry = 0;
	for(i = 0; i < 4; i++)
	{
		temp = in[i];
		result[i] = (temp << shift) | carry;
		carry = temp >> (64 - shift);
	}
	return carry;
}

static void vli_rshift1(uint64_t * vli)
{
	uint64_t * end;
	uint64_t carry;
	uint64_t temp;

	end = vli;
	carry = 0;
	vli += 4;
	while(vli-- > end)
	{
		temp = *vli;
		*vli = (temp >> 1) | carry;
		carry = temp << 63;
	}
}

static uint64_t vli_add(uint64_t * result, uint64_t * left, uint64_t * right)
{
	uint64_t carry;
	uint64_t sum;
	int i;

	carry = 0;
	for(i = 0; i < 4; i++)
	{
		sum = left[i] + right[i] + carry;
		if(sum != left[i])
			carry = (sum < left[i]);
		result[i] = sum;
	}
	return carry;
}

static uint64_t vli_sub(uint64_t * result, uint64_t * left, uint64_t * right)
{
	uint64_t borrow;
	uint64_t diff;
	int i;

	borrow = 0;
	for(i = 0; i < 4; i++)
	{
		diff = left[i] - right[i] - borrow;
		if(diff != left[i])
			borrow = (diff > left[i]);
		result[i] = diff;
	}
	return borrow;
}

static struct ecdsa256_uint128_t mul_64_64(uint64_t left, uint64_t right)
{
	struct ecdsa256_uint128_t result;
	uint64_t a0;
	uint64_t a1;
	uint64_t b0;
	uint64_t b1;
	uint64_t m0;
	uint64_t m1;
	uint64_t m2;
	uint64_t m3;

	a0 = left & 0xffffffffull;
	a1 = left >> 32;
	b0 = right & 0xffffffffull;
	b1 = right >> 32;
	m0 = a0 * b0;
	m1 = a0 * b1;
	m2 = a1 * b0;
	m3 = a1 * b1;

	m2 += (m0 >> 32);
	m2 += m1;
	if(m2 < m1)
		m3 += 0x100000000ull;
	result.m_low = (m0 & 0xffffffffull) | (m2 << 32);
	result.m_high = m3 + (m2 >> 32);

	return result;
}

static struct ecdsa256_uint128_t add_128_128(struct ecdsa256_uint128_t a, struct ecdsa256_uint128_t b)
{
	struct ecdsa256_uint128_t result;
	result.m_low = a.m_low + b.m_low;
	result.m_high = a.m_high + b.m_high + (result.m_low < a.m_low);
	return result;
}

static void vli_mult(uint64_t * result, uint64_t * left, uint64_t * right)
{
	struct ecdsa256_uint128_t product;
	struct ecdsa256_uint128_t r01;
	uint64_t r2;
	unsigned int i, k;
	unsigned int min;

	r01.m_low = 0;
	r01.m_high = 0;
	r2 = 0;

	for(k = 0; k < 7; ++k)
	{
		min = (k < 4 ? 0 : (k + 1) - 4);
		for(i = min; i <= k && i < 4; i++)
		{
			product = mul_64_64(left[i], right[k - i]);
			r01 = add_128_128(r01, product);
			r2 += (r01.m_high < product.m_high);
		}
		result[k] = r01.m_low;
		r01.m_low = r01.m_high;
		r01.m_high = r2;
		r2 = 0;
	}
	result[7] = r01.m_low;
}

static void vli_square(uint64_t *result, uint64_t *left)
{
	struct ecdsa256_uint128_t product;
	struct ecdsa256_uint128_t r01;
	uint64_t r2;
	unsigned int i, k;
	unsigned int min;

	r01.m_low = 0;
	r01.m_high = 0;
	r2 = 0;

	for(k = 0; k < 7; ++k)
	{
		min = (k < 4 ? 0 : (k + 1) - 4);
		for(i = min; i <= k && i <= k - i; i++)
		{
			product = mul_64_64(left[i], left[k - i]);
			if(i < k - i)
			{
				r2 += product.m_high >> 63;
				product.m_high = (product.m_high << 1) | (product.m_low >> 63);
				product.m_low <<= 1;
			}
			r01 = add_128_128(r01, product);
			r2 += (r01.m_high < product.m_high);
		}
		result[k] = r01.m_low;
		r01.m_low = r01.m_high;
		r01.m_high = r2;
		r2 = 0;
	}
	result[7] = r01.m_low;
}

static void vli_modadd(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t carry;

	carry = vli_add(result, left, right);
	if(carry || vli_cmp(result, mod) >= 0)
		vli_sub(result, result, mod);
}

static void vli_modsub(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t borrow;

	borrow = vli_sub(result, left, right);;
	if(borrow)
		vli_add(result, result, mod);
}

static void vli_mmod_fast(uint64_t * result, uint64_t * product, uint64_t * curve_p)
{
	uint64_t tmp[4];
	int carry;

	vli_set(result, product);

	tmp[0] = 0;
	tmp[1] = product[5] & 0xffffffff00000000ull;
	tmp[2] = product[6];
	tmp[3] = product[7];
	carry = vli_lshift(tmp, tmp, 1);
	carry += vli_add(result, result, tmp);

	tmp[1] = product[6] << 32;
	tmp[2] = (product[6] >> 32) | (product[7] << 32);
	tmp[3] = product[7] >> 32;
	carry += vli_lshift(tmp, tmp, 1);
	carry += vli_add(result, result, tmp);

	tmp[0] = product[4];
	tmp[1] = product[5] & 0xffffffff;
	tmp[2] = 0;
	tmp[3] = product[7];
	carry += vli_add(result, result, tmp);

	tmp[0] = (product[4] >> 32) | (product[5] << 32);
	tmp[1] = (product[5] >> 32) | (product[6] & 0xffffffff00000000ull);
	tmp[2] = product[7];
	tmp[3] = (product[6] >> 32) | (product[4] << 32);
	carry += vli_add(result, result, tmp);

	tmp[0] = (product[5] >> 32) | (product[6] << 32);
	tmp[1] = (product[6] >> 32);
	tmp[2] = 0;
	tmp[3] = (product[4] & 0xffffffff) | (product[5] << 32);
	carry -= vli_sub(result, result, tmp);

	tmp[0] = product[6];
	tmp[1] = product[7];
	tmp[2] = 0;
	tmp[3] = (product[4] >> 32) | (product[5] & 0xffffffff00000000ull);
	carry -= vli_sub(result, result, tmp);

	tmp[0] = (product[6] >> 32) | (product[7] << 32);
	tmp[1] = (product[7] >> 32) | (product[4] << 32);
	tmp[2] = (product[4] >> 32) | (product[5] << 32);
	tmp[3] = (product[6] << 32);
	carry -= vli_sub(result, result, tmp);

	tmp[0] = product[7];
	tmp[1] = product[4] & 0xffffffff00000000ull;
	tmp[2] = product[5];
	tmp[3] = product[6] & 0xffffffff00000000ull;
	carry -= vli_sub(result, result, tmp);

	if(carry < 0)
	{
		do
		{
			carry += vli_add(result, result, curve_p);
		} while(carry < 0);
	}
	else
	{
		while(carry || vli_cmp(curve_p, result) != 1)
		{
			carry -= vli_sub(result, result, curve_p);
		}
	}
}

static void vli_modMult_fast(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * curve_p)
{
	uint64_t product[8];
	vli_mult(product, left, right);
	vli_mmod_fast(result, product, curve_p);
}

static void vli_modSquare_fast(uint64_t * result, uint64_t * left, uint64_t * curve_p)
{
	uint64_t product[8];
	vli_square(product, left);
	vli_mmod_fast(result, product, curve_p);
}

static void vli_modinv(uint64_t * result, uint64_t * input, uint64_t * mod)
{
	uint64_t a[4], b[4], u[4], v[4];
	uint64_t carry;
	int cmpresult;

	if(vli_iszero(input))
	{
		vli_clear(result);
		return;
	}
	vli_set(a, input);
	vli_set(b, mod);
	vli_clear(u);
	u[0] = 1;
	vli_clear(v);

	while((cmpresult = vli_cmp(a, b)) != 0)
	{
		carry = 0;
		if(!(a[0] & 1))
		{
			vli_rshift1(a);
			if(u[0] & 1)
				carry = vli_add(u, u, mod);
			vli_rshift1(u);
			if(carry)
				u[4 - 1] |= 0x8000000000000000ull;
		}
		else if(!(b[0] & 1))
		{
			vli_rshift1(b);
			if(v[0] & 1)
				carry = vli_add(v, v, mod);
			vli_rshift1(v);
			if(carry)
				v[4 - 1] |= 0x8000000000000000ull;
		}
		else if(cmpresult > 0)
		{
			vli_sub(a, a, b);
			vli_rshift1(a);
			if(vli_cmp(u, v) < 0)
				vli_add(u, u, mod);
			vli_sub(u, u, v);
			if(u[0] & 1)
				carry = vli_add(u, u, mod);
			vli_rshift1(u);
			if(carry)
				u[4 - 1] |= 0x8000000000000000ull;
		}
		else
		{
			vli_sub(b, b, a);
			vli_rshift1(b);
			if(vli_cmp(v, u) < 0)
				vli_add(v, v, mod);
			vli_sub(v, v, u);
			if(v[0] & 1)
				carry = vli_add(v, v, mod);
			vli_rshift1(v);
			if(carry)
				v[4 - 1] |= 0x8000000000000000ull;
		}
	}
	vli_set(result, u);
}

static void eccpoint_double_jacobian(uint64_t * x1, uint64_t * y1, uint64_t * z1, uint64_t * curve_p)
{
	uint64_t t4[4];
	uint64_t t5[4];
	uint64_t carry;

	if(vli_iszero(z1))
		return;
	vli_modSquare_fast(t4, y1, curve_p);
	vli_modMult_fast(t5, x1, t4, curve_p);
	vli_modSquare_fast(t4, t4, curve_p);
	vli_modMult_fast(y1, y1, z1, curve_p);
	vli_modSquare_fast(z1, z1, curve_p);
	vli_modadd(x1, x1, z1, curve_p);
	vli_modadd(z1, z1, z1, curve_p);
	vli_modsub(z1, x1, z1, curve_p);
	vli_modMult_fast(x1, x1, z1, curve_p);
	vli_modadd(z1, x1, x1, curve_p);
	vli_modadd(x1, x1, z1, curve_p);
	if(vli_testbit(x1, 0))
	{
		carry = vli_add(x1, x1, curve_p);
		vli_rshift1(x1);
		x1[4 - 1] |= carry << 63;
	}
	else
	{
		vli_rshift1(x1);
	}
	vli_modSquare_fast(z1, x1, curve_p);
	vli_modsub(z1, z1, t5, curve_p);
	vli_modsub(z1, z1, t5, curve_p);
	vli_modsub(t5, t5, z1, curve_p);
	vli_modMult_fast(x1, x1, t5, curve_p);
	vli_modsub(t4, x1, t4, curve_p);
	vli_set(x1, z1);
	vli_set(z1, y1);
	vli_set(y1, t4);
}

static void apply_z(uint64_t * x1, uint64_t * y1, uint64_t * z, uint64_t * curve_p)
{
	uint64_t t1[4];

	vli_modSquare_fast(t1, z, curve_p);
	vli_modMult_fast(x1, x1, t1, curve_p);
	vli_modMult_fast(t1, t1, z, curve_p);
	vli_modMult_fast(y1, y1, t1, curve_p);
}

static void xycz_add(uint64_t * x1, uint64_t * y1, uint64_t * x2, uint64_t * y2, uint64_t * curve_p)
{
	uint64_t t5[4];

	vli_modsub(t5, x2, x1, curve_p);
	vli_modSquare_fast(t5, t5, curve_p);
	vli_modMult_fast(x1, x1, t5, curve_p);
	vli_modMult_fast(x2, x2, t5, curve_p);
	vli_modsub(y2, y2, y1, curve_p);
	vli_modSquare_fast(t5, y2, curve_p);
	vli_modsub(t5, t5, x1, curve_p);
	vli_modsub(t5, t5, x2, curve_p);
	vli_modsub(x2, x2, x1, curve_p);
	vli_modMult_fast(y1, y1, x2, curve_p);
	vli_modsub(x2, x1, t5, curve_p);
	vli_modMult_fast(y2, y2, x2, curve_p);
	vli_modsub(y2, y2, y1, curve_p);
	vli_set(x2, t5);
}

static void ecc_bytes2native(uint64_t * native, const uint8_t * bytes)
{
	const uint8_t * digit;
	int i;

	for(i = 0; i < 4; i++)
	{
		digit = bytes + ((3 - i) << 3);
		native[i] = ((uint64_t)digit[0] << 56) | ((uint64_t)digit[1] << 48) | ((uint64_t)digit[2] << 40) | ((uint64_t)digit[3] << 32)
		        | ((uint64_t)digit[4] << 24) | ((uint64_t)digit[5] << 16) | ((uint64_t)digit[6] << 8) | (uint64_t)digit[7];
	}
}

static void mod_sqrt(uint64_t * a, uint64_t * curve_p)
{
	uint64_t p1[4];
	uint64_t result[4];
	int i;

	p1[0] = 1;
	p1[1] = 0;
	p1[2] = 0;
	p1[3] = 0;
	result[0] = 1;
	result[1] = 0;
	result[2] = 0;
	result[3] = 0;

	vli_add(p1, curve_p, p1);
	for(i = vli_numbits(p1) - 1; i > 1; --i)
	{
		vli_modSquare_fast(result, result, curve_p);
		if(vli_testbit(p1, i))
			vli_modMult_fast(result, result, a, curve_p);
	}
	vli_set(a, result);
}

static void ecc_point_decompress(struct ecdsa256_point_t * point, const uint8_t * compressed, uint64_t * curve_p)
{
	uint64_t curve_b[4];
	uint64_t _3[4];

	curve_b[0] = 0x3bce3c3e27d2604bull;
	curve_b[1] = 0x651d06b0cc53b0f6ull;
	curve_b[2] = 0xb3ebbd55769886bcull;
	curve_b[3] = 0x5ac635d8aa3a93e7ull;
	_3[0] = 3;
	_3[1] = 0;
	_3[2] = 0;
	_3[3] = 0;

	ecc_bytes2native(point->x, compressed + 1);
	vli_modSquare_fast(point->y, point->x, curve_p);
	vli_modsub(point->y, point->y, _3, curve_p);
	vli_modMult_fast(point->y, point->y, point->x, curve_p);
	vli_modadd(point->y, point->y, curve_b, curve_p);
	mod_sqrt(point->y, curve_p);
	if((point->y[0] & 0x01) != (compressed[0] & 0x01))
		vli_sub(point->y, curve_p, point->y);
}

static void vli_modmult(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t product[8];
	uint64_t modmultiple[8];
	uint64_t carry;
	unsigned int digitshift, bitshift;
	unsigned int productbits;
	unsigned int modbits;
	int l_cmp;

	modbits = vli_numbits(mod);
	vli_mult(product, left, right);
	productbits = vli_numbits(product + 4);
	if(productbits)
		productbits += 256;
	else
		productbits = vli_numbits(product);
	if(productbits < modbits)
	{
		vli_set(result, product);
		return;
	}
	vli_clear(modmultiple);
	vli_clear(modmultiple + 4);
	digitshift = (productbits - modbits) >> 6;
	bitshift = (productbits - modbits) & 0x3f;
	if(bitshift)
		modmultiple[digitshift + 4] = vli_lshift(modmultiple + digitshift, mod, bitshift);
	else
		vli_set(modmultiple + digitshift, mod);
	vli_clear(result);
	result[0] = 1;
	while(productbits > 256 || vli_cmp(modmultiple, mod) >= 0)
	{
		l_cmp = vli_cmp(modmultiple + 4, product + 4);
		if(l_cmp < 0 || (l_cmp == 0 && vli_cmp(modmultiple, product) <= 0))
		{
			if(vli_sub(product, product, modmultiple))
				vli_sub(product + 4, product + 4, result);
			vli_sub(product + 4, product + 4, modmultiple + 4);
		}
		carry = (modmultiple[4] & 0x01) << 63;
		vli_rshift1(modmultiple + 4);
		vli_rshift1(modmultiple);
		modmultiple[4 - 1] |= carry;
		--productbits;
	}
	vli_set(result, product);
}

int sys_verify(char * public, char * sha256, char * signature)
{
	struct ecdsa256_point_t curve_g;
	struct ecdsa256_point_t * points[4];
	uint64_t curve_p[4];
	uint64_t curve_n[4];
	uint64_t u1[4], u2[4];
	uint64_t z[4];
	struct ecdsa256_point_t lpublic, sum;
	struct ecdsa256_point_t * point;
	uint64_t rx[4];
	uint64_t ry[4];
	uint64_t tx[4];
	uint64_t ty[4];
	uint64_t tz[4];
	uint64_t r[4], s[4];
	unsigned int numbits;
	int index;
	int i;

	curve_g.x[0] = 0xf4a13945d898c296ull;
	curve_g.x[1] = 0x77037d812deb33a0ull;
	curve_g.x[2] = 0xf8bce6e563a440f2ull;
	curve_g.x[3] = 0x6b17d1f2e12c4247ull;
	curve_g.y[0] = 0xcbb6406837bf51f5ull;
	curve_g.y[1] = 0x2bce33576b315eceull;
	curve_g.y[2] = 0x8ee7eb4a7c0f9e16ull;
	curve_g.y[3] = 0x4fe342e2fe1a7f9bull;
	curve_p[0] = 0xffffffffffffffffull;
	curve_p[1] = 0x00000000ffffffffull;
	curve_p[2] = 0x0000000000000000ull;
	curve_p[3] = 0xffffffff00000001ull;
	curve_n[0] = 0xf3b9cac2fc632551ull;
	curve_n[1] = 0xbce6faada7179e84ull;
	curve_n[2] = 0xffffffffffffffffull;
	curve_n[3] = 0xffffffff00000000ull;

	ecc_point_decompress(&lpublic, (uint8_t *)public, curve_p);
	ecc_bytes2native(r, (uint8_t *)signature);
	ecc_bytes2native(s, (uint8_t *)signature + 32);
	if(vli_iszero(r) || vli_iszero(s))
		return 0;
	if(vli_cmp(curve_n, r) != 1 || vli_cmp(curve_n, s) != 1)
		return 0;
	vli_modinv(z, s, curve_n);
	ecc_bytes2native(u1, (uint8_t *)sha256);
	vli_modmult(u1, u1, z, curve_n);
	vli_modmult(u2, r, z, curve_n);
	vli_set(sum.x, lpublic.x);
	vli_set(sum.y, lpublic.y);
	vli_set(tx, curve_g.x);
	vli_set(ty, curve_g.y);
	vli_modsub(z, sum.x, tx, curve_p);
	xycz_add(tx, ty, sum.x, sum.y, curve_p);
	vli_modinv(z, z, curve_p);
	apply_z(sum.x, sum.y, z, curve_p);
	points[0] = NULL;
	points[1] = &curve_g;
	points[2] = &lpublic;
	points[3] = &sum;
	numbits = vli_numbits(u1) > vli_numbits(u2) ? vli_numbits(u1) : vli_numbits(u2);
	point = points[(!!vli_testbit(u1, numbits - 1)) | ((!!vli_testbit(u2, numbits - 1)) << 1)];
	vli_set(rx, point->x);
	vli_set(ry, point->y);
	vli_clear(z);
	z[0] = 1;
	for(i = numbits - 2; i >= 0; --i)
	{
		eccpoint_double_jacobian(rx, ry, z, curve_p);
		index = (!!vli_testbit(u1, i)) | ((!!vli_testbit(u2, i)) << 1);
		point = points[index];
		if(point)
		{
			vli_set(tx, point->x);
			vli_set(ty, point->y);
			apply_z(tx, ty, z, curve_p);
			vli_modsub(tz, rx, tx, curve_p);
			xycz_add(tx, ty, rx, ry, curve_p);
			vli_modMult_fast(z, z, tz, curve_p);
		}
	}
	vli_modinv(z, z, curve_p);
	apply_z(rx, ry, z, curve_p);
	if(vli_cmp(curve_n, rx) != 1)
		vli_sub(rx, rx, curve_n);
	return (vli_cmp(rx, r) == 0);
}
