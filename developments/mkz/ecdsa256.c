#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ecdsa256.h>

#define ECDSA256_NUM_DIGITS	(ECDSA256_BYTES / 8)
#define MAX_RETRY			(16)

struct ecdsa256_uint128_t {
	uint64_t m_low;
	uint64_t m_high;
};

struct ecdsa256_point_t {
	uint64_t x[ECDSA256_NUM_DIGITS];
	uint64_t y[ECDSA256_NUM_DIGITS];
};

static uint64_t curve_p[ECDSA256_NUM_DIGITS] = { 0xFFFFFFFFFFFFFFFFull, 0x00000000FFFFFFFFull, 0x0000000000000000ull, 0xFFFFFFFF00000001ull };
static uint64_t curve_b[ECDSA256_NUM_DIGITS] = { 0x3BCE3C3E27D2604Bull, 0x651D06B0CC53B0F6ull, 0xB3EBBD55769886BCull, 0x5AC635D8AA3A93E7ull };
static uint64_t curve_n[ECDSA256_NUM_DIGITS] = { 0xF3B9CAC2FC632551ull, 0xBCE6FAADA7179E84ull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFF00000000ull };
static struct ecdsa256_point_t curve_g = { { 0xF4A13945D898C296ull, 0x77037D812DEB33A0ull, 0xF8BCE6E563A440F2ull, 0x6B17D1F2E12C4247ull }, { 0xCBB6406837BF51F5ull, 0x2BCE33576B315ECEull, 0x8EE7EB4A7C0F9E16ull, 0x4FE342E2FE1A7F9Bull } };

static int get_random_number(uint64_t * vli)
{
	uint64_t v;
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		v = (uint64_t)(rand() & 0xffff) << 0;
		v |= (uint64_t)(rand() & 0xffff) << 16;
		v |= (uint64_t)(rand() & 0xffff) << 32;
		v |= (uint64_t)(rand() & 0xffff) << 48;
		vli[i] = v;
	}
	return 1;
}

static void vli_clear(uint64_t * vli)
{
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
		vli[i] = 0;
}

static int vli_iszero(uint64_t * vli)
{
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		if(vli[i])
			return 0;
	}
	return 1;
}

static uint64_t vli_testbit(uint64_t * vli, unsigned int bit)
{
	return (vli[bit / 64] & ((uint64_t)1 << (bit % 64)));
}

static unsigned int vli_numdigits(uint64_t * vli)
{
    int i;

    for(i = ECDSA256_NUM_DIGITS - 1; i >= 0 && vli[i] == 0; --i);
    return (i + 1);
}

static unsigned int vli_numbits(uint64_t * vli)
{
	uint64_t digit;
	unsigned int i;
	unsigned int n = vli_numdigits(vli);

	if(n == 0)
		return 0;
	digit = vli[n - 1];
	for(i = 0; digit; i++)
		digit >>= 1;
	return ((n - 1) * 64 + i);
}

static void vli_set(uint64_t * dst, uint64_t * src)
{
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
		dst[i] = src[i];
}

static int vli_cmp(uint64_t * left, uint64_t * right)
{
	int i;

	for(i = ECDSA256_NUM_DIGITS - 1; i >= 0; --i)
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
	uint64_t carry = 0;
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		uint64_t temp = in[i];
		result[i] = (temp << shift) | carry;
		carry = temp >> (64 - shift);
	}
	return carry;
}

static void vli_rshift1(uint64_t * vli)
{
	uint64_t * end = vli;
	uint64_t carry = 0;

	vli += ECDSA256_NUM_DIGITS;
	while(vli-- > end)
	{
		uint64_t temp = *vli;
		*vli = (temp >> 1) | carry;
		carry = temp << 63;
	}
}

static uint64_t vli_add(uint64_t * result, uint64_t * left, uint64_t * right)
{
	uint64_t carry = 0;
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		uint64_t sum = left[i] + right[i] + carry;
		if(sum != left[i])
			carry = (sum < left[i]);
		result[i] = sum;
	}
	return carry;
}

static uint64_t vli_sub(uint64_t * result, uint64_t * left, uint64_t * right)
{
	uint64_t borrow = 0;
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		uint64_t diff = left[i] - right[i] - borrow;
		if(diff != left[i])
			borrow = (diff > left[i]);
		result[i] = diff;
	}
	return borrow;
}

static struct ecdsa256_uint128_t mul_64_64(uint64_t left, uint64_t right)
{
	struct ecdsa256_uint128_t result;

	uint64_t a0 = left & 0xffffffffull;
	uint64_t a1 = left >> 32;
	uint64_t b0 = right & 0xffffffffull;
	uint64_t b1 = right >> 32;
	uint64_t m0 = a0 * b0;
	uint64_t m1 = a0 * b1;
	uint64_t m2 = a1 * b0;
	uint64_t m3 = a1 * b1;

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
	struct ecdsa256_uint128_t r01 = { 0, 0 };
	uint64_t r2 = 0;
	unsigned int i, k;

	for(k = 0; k < ECDSA256_NUM_DIGITS * 2 - 1; ++k)
	{
		unsigned int min = (k < ECDSA256_NUM_DIGITS ? 0 : (k + 1) - ECDSA256_NUM_DIGITS);
		for(i = min; i <= k && i < ECDSA256_NUM_DIGITS; i++)
		{
			struct ecdsa256_uint128_t product = mul_64_64(left[i], right[k - i]);
			r01 = add_128_128(r01, product);
			r2 += (r01.m_high < product.m_high);
		}
		result[k] = r01.m_low;
		r01.m_low = r01.m_high;
		r01.m_high = r2;
		r2 = 0;
	}
	result[ECDSA256_NUM_DIGITS * 2 - 1] = r01.m_low;
}

static void vli_square(uint64_t *result, uint64_t *left)
{
	struct ecdsa256_uint128_t r01 = { 0, 0 };
	uint64_t r2 = 0;
	unsigned int i, k;

	for(k = 0; k < ECDSA256_NUM_DIGITS * 2 - 1; ++k)
	{
		unsigned int min = (k < ECDSA256_NUM_DIGITS ? 0 : (k + 1) - ECDSA256_NUM_DIGITS);
		for(i = min; i <= k && i <= k - i; i++)
		{
			struct ecdsa256_uint128_t product = mul_64_64(left[i], left[k - i]);
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
	result[ECDSA256_NUM_DIGITS * 2 - 1] = r01.m_low;
}

static void vli_modadd(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t carry = vli_add(result, left, right);
	if(carry || vli_cmp(result, mod) >= 0)
		vli_sub(result, result, mod);
}

static void vli_modsub(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t borrow = vli_sub(result, left, right);
	if(borrow)
		vli_add(result, result, mod);
}

static void vli_mmod_fast(uint64_t * result, uint64_t * product)
{
	uint64_t tmp[ECDSA256_NUM_DIGITS];
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

static void vli_modMult_fast(uint64_t * result, uint64_t * left, uint64_t * right)
{
	uint64_t product[2 * ECDSA256_NUM_DIGITS];
	vli_mult(product, left, right);
	vli_mmod_fast(result, product);
}

static void vli_modSquare_fast(uint64_t * result, uint64_t * left)
{
	uint64_t product[2 * ECDSA256_NUM_DIGITS];
	vli_square(product, left);
	vli_mmod_fast(result, product);
}

static void vli_modinv(uint64_t * result, uint64_t * input, uint64_t * mod)
{
	uint64_t a[ECDSA256_NUM_DIGITS], b[ECDSA256_NUM_DIGITS], u[ECDSA256_NUM_DIGITS], v[ECDSA256_NUM_DIGITS];
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
				u[ECDSA256_NUM_DIGITS - 1] |= 0x8000000000000000ull;
		}
		else if(!(b[0] & 1))
		{
			vli_rshift1(b);
			if(v[0] & 1)
				carry = vli_add(v, v, mod);
			vli_rshift1(v);
			if(carry)
				v[ECDSA256_NUM_DIGITS - 1] |= 0x8000000000000000ull;
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
				u[ECDSA256_NUM_DIGITS - 1] |= 0x8000000000000000ull;
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
				v[ECDSA256_NUM_DIGITS - 1] |= 0x8000000000000000ull;
		}
	}
	vli_set(result, u);
}

static int eccpoint_iszero(struct ecdsa256_point_t * point)
{
	return (vli_iszero(point->x) && vli_iszero(point->y));
}

static void eccpoint_double_jacobian(uint64_t * x1, uint64_t * y1, uint64_t * z1)
{
	uint64_t t4[ECDSA256_NUM_DIGITS];
	uint64_t t5[ECDSA256_NUM_DIGITS];

	if(vli_iszero(z1))
		return;
	vli_modSquare_fast(t4, y1);
	vli_modMult_fast(t5, x1, t4);
	vli_modSquare_fast(t4, t4);
	vli_modMult_fast(y1, y1, z1);
	vli_modSquare_fast(z1, z1);
	vli_modadd(x1, x1, z1, curve_p);
	vli_modadd(z1, z1, z1, curve_p);
	vli_modsub(z1, x1, z1, curve_p);
	vli_modMult_fast(x1, x1, z1);
	vli_modadd(z1, x1, x1, curve_p);
	vli_modadd(x1, x1, z1, curve_p);
	if(vli_testbit(x1, 0))
	{
		uint64_t carry = vli_add(x1, x1, curve_p);
		vli_rshift1(x1);
		x1[ECDSA256_NUM_DIGITS - 1] |= carry << 63;
	}
	else
	{
		vli_rshift1(x1);
	}
	vli_modSquare_fast(z1, x1);
	vli_modsub(z1, z1, t5, curve_p);
	vli_modsub(z1, z1, t5, curve_p);
	vli_modsub(t5, t5, z1, curve_p);
	vli_modMult_fast(x1, x1, t5);
	vli_modsub(t4, x1, t4, curve_p);
	vli_set(x1, z1);
	vli_set(z1, y1);
	vli_set(y1, t4);
}

static void apply_z(uint64_t * x1, uint64_t * y1, uint64_t * z)
{
	uint64_t t1[ECDSA256_NUM_DIGITS];

	vli_modSquare_fast(t1, z);
	vli_modMult_fast(x1, x1, t1);
	vli_modMult_fast(t1, t1, z);
	vli_modMult_fast(y1, y1, t1);
}

static void xycz_initial_double(uint64_t * x1, uint64_t * y1, uint64_t * x2, uint64_t * y2, uint64_t * initialz)
{
	uint64_t z[ECDSA256_NUM_DIGITS];

	vli_set(x2, x1);
	vli_set(y2, y1);
	vli_clear(z);
	z[0] = 1;
	if(initialz)
		vli_set(z, initialz);
	apply_z(x1, y1, z);
	eccpoint_double_jacobian(x1, y1, z);
	apply_z(x2, y2, z);
}

static void xycz_add(uint64_t * x1, uint64_t * y1, uint64_t * x2, uint64_t * y2)
{
	uint64_t t5[ECDSA256_NUM_DIGITS];

	vli_modsub(t5, x2, x1, curve_p);
	vli_modSquare_fast(t5, t5);
	vli_modMult_fast(x1, x1, t5);
	vli_modMult_fast(x2, x2, t5);
	vli_modsub(y2, y2, y1, curve_p);
	vli_modSquare_fast(t5, y2);
	vli_modsub(t5, t5, x1, curve_p);
	vli_modsub(t5, t5, x2, curve_p);
	vli_modsub(x2, x2, x1, curve_p);
	vli_modMult_fast(y1, y1, x2);
	vli_modsub(x2, x1, t5, curve_p);
	vli_modMult_fast(y2, y2, x2);
	vli_modsub(y2, y2, y1, curve_p);
	vli_set(x2, t5);
}

static void xycz_addc(uint64_t * x1, uint64_t * y1, uint64_t * x2, uint64_t * y2)
{
	uint64_t t5[ECDSA256_NUM_DIGITS];
	uint64_t t6[ECDSA256_NUM_DIGITS];
	uint64_t t7[ECDSA256_NUM_DIGITS];

	vli_modsub(t5, x2, x1, curve_p);
	vli_modSquare_fast(t5, t5);
	vli_modMult_fast(x1, x1, t5);
	vli_modMult_fast(x2, x2, t5);
	vli_modadd(t5, y2, y1, curve_p);
	vli_modsub(y2, y2, y1, curve_p);
	vli_modsub(t6, x2, x1, curve_p);
	vli_modMult_fast(y1, y1, t6);
	vli_modadd(t6, x1, x2, curve_p);
	vli_modSquare_fast(x2, y2);
	vli_modsub(x2, x2, t6, curve_p);
	vli_modsub(t7, x1, x2, curve_p);
	vli_modMult_fast(y2, y2, t7);
	vli_modsub(y2, y2, y1, curve_p);
	vli_modSquare_fast(t7, t5);
	vli_modsub(t7, t7, t6, curve_p);
	vli_modsub(t6, t7, x1, curve_p);
	vli_modMult_fast(t6, t6, t5);
	vli_modsub(y1, t6, y1, curve_p);
	vli_set(x1, t7);
}

static void eccpoint_mult(struct ecdsa256_point_t * result, struct ecdsa256_point_t * point, uint64_t * scalar, uint64_t * initialz)
{
	uint64_t Rx[2][ECDSA256_NUM_DIGITS];
	uint64_t Ry[2][ECDSA256_NUM_DIGITS];
	uint64_t z[ECDSA256_NUM_DIGITS];
	int i, nb;

	vli_set(Rx[1], point->x);
	vli_set(Ry[1], point->y);
	xycz_initial_double(Rx[1], Ry[1], Rx[0], Ry[0], initialz);
	for(i = vli_numbits(scalar) - 2; i > 0; --i)
	{
		nb = !vli_testbit(scalar, i);
		xycz_addc(Rx[1 - nb], Ry[1 - nb], Rx[nb], Ry[nb]);
		xycz_add(Rx[nb], Ry[nb], Rx[1 - nb], Ry[1 - nb]);
	}
	nb = !vli_testbit(scalar, 0);
	xycz_addc(Rx[1 - nb], Ry[1 - nb], Rx[nb], Ry[nb]);
	vli_modsub(z, Rx[1], Rx[0], curve_p);
	vli_modMult_fast(z, z, Ry[1 - nb]);
	vli_modMult_fast(z, z, point->x);
	vli_modinv(z, z, curve_p);
	vli_modMult_fast(z, z, point->y);
	vli_modMult_fast(z, z, Rx[1 - nb]);
	xycz_add(Rx[nb], Ry[nb], Rx[1 - nb], Ry[1 - nb]);
	apply_z(Rx[0], Ry[0], z);
	vli_set(result->x, Rx[0]);
	vli_set(result->y, Ry[0]);
}

static void ecc_bytes2native(uint64_t * native, const uint8_t * bytes)
{
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		const uint8_t * digit = bytes + 8 * (ECDSA256_NUM_DIGITS - 1 - i);
		native[i] = ((uint64_t)digit[0] << 56) | ((uint64_t)digit[1] << 48) | ((uint64_t)digit[2] << 40) | ((uint64_t)digit[3] << 32)
		        | ((uint64_t)digit[4] << 24) | ((uint64_t)digit[5] << 16) | ((uint64_t)digit[6] << 8) | (uint64_t)digit[7];
	}
}

static void ecc_native2bytes(uint8_t * bytes, const uint64_t * native)
{
	int i;

	for(i = 0; i < ECDSA256_NUM_DIGITS; i++)
	{
		uint8_t *digit = bytes + 8 * (ECDSA256_NUM_DIGITS - 1 - i);
		digit[0] = native[i] >> 56;
		digit[1] = native[i] >> 48;
		digit[2] = native[i] >> 40;
		digit[3] = native[i] >> 32;
		digit[4] = native[i] >> 24;
		digit[5] = native[i] >> 16;
		digit[6] = native[i] >> 8;
		digit[7] = native[i];
	}
}

static void mod_sqrt(uint64_t * a)
{
	uint64_t p1[ECDSA256_NUM_DIGITS] = { 1 };
	uint64_t result[ECDSA256_NUM_DIGITS] = { 1 };
	int i;

	vli_add(p1, curve_p, p1);
	for(i = vli_numbits(p1) - 1; i > 1; --i)
	{
		vli_modSquare_fast(result, result);
		if(vli_testbit(p1, i))
			vli_modMult_fast(result, result, a);
	}
	vli_set(a, result);
}

static void ecc_point_decompress(struct ecdsa256_point_t * point, const uint8_t * compressed)
{
	uint64_t _3[ECDSA256_NUM_DIGITS] = { 3 };

	ecc_bytes2native(point->x, compressed + 1);
	vli_modSquare_fast(point->y, point->x);
	vli_modsub(point->y, point->y, _3, curve_p);
	vli_modMult_fast(point->y, point->y, point->x);
	vli_modadd(point->y, point->y, curve_b, curve_p);
	mod_sqrt(point->y);
	if((point->y[0] & 0x01) != (compressed[0] & 0x01))
		vli_sub(point->y, curve_p, point->y);
}

static void vli_modmult(uint64_t * result, uint64_t * left, uint64_t * right, uint64_t * mod)
{
	uint64_t product[2 * ECDSA256_NUM_DIGITS];
	uint64_t modmultiple[2 * ECDSA256_NUM_DIGITS];
	unsigned int digitshift, bitshift;
	unsigned int productbits;
	unsigned int modbits = vli_numbits(mod);

	vli_mult(product, left, right);
	productbits = vli_numbits(product + ECDSA256_NUM_DIGITS);
	if(productbits)
		productbits += ECDSA256_NUM_DIGITS * 64;
	else
		productbits = vli_numbits(product);
	if(productbits < modbits)
	{
		vli_set(result, product);
		return;
	}
	vli_clear(modmultiple);
	vli_clear(modmultiple + ECDSA256_NUM_DIGITS);
	digitshift = (productbits - modbits) / 64;
	bitshift = (productbits - modbits) % 64;
	if(bitshift)
		modmultiple[digitshift + ECDSA256_NUM_DIGITS] = vli_lshift(modmultiple + digitshift, mod, bitshift);
	else
		vli_set(modmultiple + digitshift, mod);
	vli_clear(result);
	result[0] = 1;
	while(productbits > ECDSA256_NUM_DIGITS * 64 || vli_cmp(modmultiple, mod) >= 0)
	{
		int l_cmp = vli_cmp(modmultiple + ECDSA256_NUM_DIGITS, product + ECDSA256_NUM_DIGITS);
		if(l_cmp < 0 || (l_cmp == 0 && vli_cmp(modmultiple, product) <= 0))
		{
			if(vli_sub(product, product, modmultiple))
				vli_sub(product + ECDSA256_NUM_DIGITS, product + ECDSA256_NUM_DIGITS, result);
			vli_sub(product + ECDSA256_NUM_DIGITS, product + ECDSA256_NUM_DIGITS, modmultiple + ECDSA256_NUM_DIGITS);
		}
		uint64_t carry = (modmultiple[ECDSA256_NUM_DIGITS] & 0x01) << 63;
		vli_rshift1(modmultiple + ECDSA256_NUM_DIGITS);
		vli_rshift1(modmultiple);
		modmultiple[ECDSA256_NUM_DIGITS - 1] |= carry;
		--productbits;
	}
	vli_set(result, product);
}

static unsigned int umax(unsigned int a, unsigned int b)
{
	return (a > b ? a : b);
}

int ecdh256_keygen(const uint8_t * public, const uint8_t * private, uint8_t * shared)
{
	struct ecdsa256_point_t lpublic;
	struct ecdsa256_point_t product;
	uint64_t lprivate[ECDSA256_NUM_DIGITS];
	uint64_t lrandom[ECDSA256_NUM_DIGITS];

	if(!get_random_number(lrandom))
		return 0;
	ecc_point_decompress(&lpublic, public);
	ecc_bytes2native(lprivate, private);
	eccpoint_mult(&product, &lpublic, lprivate, lrandom);
	ecc_native2bytes(shared, product.x);
	return !eccpoint_iszero(&product);
}

int ecdsa256_keygen(uint8_t * public, uint8_t * private)
{
	uint64_t lprivate[ECDSA256_NUM_DIGITS];
	struct ecdsa256_point_t lpublic;
	int retry = 0;

	do
	{
		if(!get_random_number(lprivate) || (retry++ >= MAX_RETRY))
			return 0;
		if(vli_iszero(lprivate))
			continue;
		if(vli_cmp(curve_n, lprivate) != 1)
			vli_sub(lprivate, lprivate, curve_n);
		eccpoint_mult(&lpublic, &curve_g, lprivate, NULL);
	} while(eccpoint_iszero(&lpublic));
	ecc_native2bytes(private, lprivate);
	ecc_native2bytes(public + 1, lpublic.x);
	public[0] = 2 + (lpublic.y[0] & 0x01);

	return 1;
}

int ecdsa256_sign(const uint8_t * private, const uint8_t * sha256, uint8_t * signature)
{
	uint64_t k[ECDSA256_NUM_DIGITS];
	uint64_t tmp[ECDSA256_NUM_DIGITS];
	uint64_t s[ECDSA256_NUM_DIGITS];
	struct ecdsa256_point_t p;
	int retry = 0;

	do
	{
		if(!get_random_number(k) || (retry++ >= MAX_RETRY))
			return 0;
		if(vli_iszero(k))
			continue;
		if(vli_cmp(curve_n, k) != 1)
			vli_sub(k, k, curve_n);
		eccpoint_mult(&p, &curve_g, k, NULL);
		if(vli_cmp(curve_n, p.x) != 1)
			vli_sub(p.x, p.x, curve_n);
	} while(vli_iszero(p.x));
	ecc_native2bytes(signature, p.x);
	ecc_bytes2native(tmp, private);
	vli_modmult(s, p.x, tmp, curve_n);
	ecc_bytes2native(tmp, sha256);
	vli_modadd(s, tmp, s, curve_n);
	vli_modinv(k, k, curve_n);
	vli_modmult(s, s, k, curve_n);
	ecc_native2bytes(signature + ECDSA256_BYTES, s);

	return 1;
}

int ecdsa256_verify(const uint8_t * public, const uint8_t * sha256, const uint8_t * signature)
{
	uint64_t u1[ECDSA256_NUM_DIGITS], u2[ECDSA256_NUM_DIGITS];
	uint64_t z[ECDSA256_NUM_DIGITS];
	struct ecdsa256_point_t lpublic, sum;
	uint64_t rx[ECDSA256_NUM_DIGITS];
	uint64_t ry[ECDSA256_NUM_DIGITS];
	uint64_t tx[ECDSA256_NUM_DIGITS];
	uint64_t ty[ECDSA256_NUM_DIGITS];
	uint64_t tz[ECDSA256_NUM_DIGITS];
	uint64_t r[ECDSA256_NUM_DIGITS], s[ECDSA256_NUM_DIGITS];
	int i;

	ecc_point_decompress(&lpublic, public);
	ecc_bytes2native(r, signature);
	ecc_bytes2native(s, signature + ECDSA256_BYTES);
	if(vli_iszero(r) || vli_iszero(s))
		return 0;
	if(vli_cmp(curve_n, r) != 1 || vli_cmp(curve_n, s) != 1)
		return 0;
	vli_modinv(z, s, curve_n);
	ecc_bytes2native(u1, sha256);
	vli_modmult(u1, u1, z, curve_n);
	vli_modmult(u2, r, z, curve_n);
	vli_set(sum.x, lpublic.x);
	vli_set(sum.y, lpublic.y);
	vli_set(tx, curve_g.x);
	vli_set(ty, curve_g.y);
	vli_modsub(z, sum.x, tx, curve_p);
	xycz_add(tx, ty, sum.x, sum.y);
	vli_modinv(z, z, curve_p);
	apply_z(sum.x, sum.y, z);
	struct ecdsa256_point_t * points[4] = { NULL, &curve_g, &lpublic, &sum };
	unsigned int numbits = umax(vli_numbits(u1), vli_numbits(u2));
	struct ecdsa256_point_t * point = points[(!!vli_testbit(u1, numbits - 1)) | ((!!vli_testbit(u2, numbits - 1)) << 1)];
	vli_set(rx, point->x);
	vli_set(ry, point->y);
	vli_clear(z);
	z[0] = 1;
	for(i = numbits - 2; i >= 0; --i)
	{
		eccpoint_double_jacobian(rx, ry, z);
		int index = (!!vli_testbit(u1, i)) | ((!!vli_testbit(u2, i)) << 1);
		struct ecdsa256_point_t * point = points[index];
		if(point)
		{
			vli_set(tx, point->x);
			vli_set(ty, point->y);
			apply_z(tx, ty, z);
			vli_modsub(tz, rx, tx, curve_p);
			xycz_add(tx, ty, rx, ry);
			vli_modMult_fast(z, z, tz);
		}
	}
	vli_modinv(z, z, curve_p);
	apply_z(rx, ry, z);
	if(vli_cmp(curve_n, rx) != 1)
		vli_sub(rx, rx, curve_n);
	return (vli_cmp(rx, r) == 0);
}
