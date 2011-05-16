/*
 * lib/libc/rsa.c
 */

#include <xboot.h>
#include <types.h>
#include <stddef.h>
#include <rsa.h>

/*
 * a[] -= mod
 */
static void subM(const struct rsa_public_key * key, u32_t * a)
{
	s64_t A = 0;
    s32_t i;

    for(i = 0; i < key->len; ++i)
    {
        A += (u64_t)a[i] - key->n[i];
        a[i] = (u32_t)A;
        A >>= 32;
    }
}

/*
 * return a[] >= mod
 */
static s32_t geM(const struct rsa_public_key * key, const u32_t * a)
{
	s32_t i;

	for(i = key->len; i;)
	{
        --i;
        if (a[i] < key->n[i])
        	return 0;
        if (a[i] > key->n[i])
        	return 1;
    }

	/* equal */
    return 1;
}

/*
 * montgomery c[] += a * b[] / R % mod
 */
static void montMulAdd(const struct rsa_public_key * key, u32_t * c, const u32_t a, const u32_t * b)
{
	u64_t A = (u64_t)a * b[0] + c[0];
    u32_t d0 = (u32_t)A * key->n0inv;
    u64_t B = (u64_t)d0 * key->n[0] + (u32_t)A;
    s32_t i;

    for(i = 1; i < key->len; ++i)
    {
        A = (A >> 32) + (u64_t)a * b[i] + c[i];
        B = (B >> 32) + (u64_t)d0 * key->n[i] + (u32_t)A;
        c[i - 1] = (u32_t)B;
    }

    A = (A >> 32) + (B >> 32);

    c[i - 1] = (u32_t)A;

    if(A >> 32)
    {
        subM(key, c);
    }
}

/*
 * montgomery c[] = a[] * b[] / R % mod
 */
static void montMul(const struct rsa_public_key * key, u32_t * c, const u32_t * a, const u32_t * b)
{
	s32_t i;

    for(i = 0; i < key->len; ++i)
	{
    	c[i] = 0;
    }

    for(i = 0; i < key->len; ++i)
    {
        montMulAdd(key, c, a[i], b);
    }
}

/*
 * in-place public exponentiation.
 * input and output big-endian byte array in inout.
 */
static void modpow3(const struct rsa_public_key * key, u8_t * inout)
{
    u32_t a[256/sizeof(u32_t)];
    u32_t aR[256/sizeof(u32_t)];
    u32_t aaR[256/sizeof(u32_t)];
    u32_t *aaa = aR;
    s32_t i;

    /*
     * convert from big endian byte array to little endian word array.
     */
    for(i = 0; i < key->len; ++i)
    {
		u32_t tmp = (inout[((key->len - 1 - i) * 4) + 0] << 24) |	\
            (inout[((key->len - 1 - i) * 4) + 1] << 16) | 			\
            (inout[((key->len - 1 - i) * 4) + 2] << 8) |			\
            (inout[((key->len - 1 - i) * 4) + 3] << 0);
        a[i] = tmp;
    }

    /* aR = a * RR / R mod M   */
    montMul(key, aR, a, key->rr);
    /* aaR = aR * aR / R mod M */
    montMul(key, aaR, aR, aR);
    /* aaa = aaR * a / R mod M */
    montMul(key, aaa, aaR, a);

    /* make sure aaa < mod; aaa is at most 1x mod too large */
    if(geM(key, aaa))
    {
        subM(key, aaa);
    }

    /* convert to bigendian byte array */
    for(i = key->len - 1; i >= 0; --i)
    {
    	u32_t tmp = aaa[i];
        *inout++ = tmp >> 24;
        *inout++ = tmp >> 16;
        *inout++ = tmp >> 8;
        *inout++ = tmp >> 0;
    }
}

/*
 * expected PKCS1.5 signature padding bytes, for a keytool RSA signature.
 * has the 0-length optional parameter encoded in the ASN1 (as opposed to the
 * other flavor which omits the optional parameter entirely). This code does not
 * accept signatures without the optional parameter.
*/
static const u8_t padding[256 - 20] = {
    0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,
    0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,
    0x04,0x14
};

/*
 * verify a 2048 bit RSA PKCS1.5 signature against an expected SHA-1 hash.
 * returns FALSE on failure, TRUE on success.
*/
bool_t rsa_verify(const struct rsa_public_key * key, const u8_t * signature, const u8_t * sha)
{
    u8_t buf[256];
    s32_t i;

    if(key->len != (256/sizeof(u32_t)))
    {
    	/* wrong key passed in */
        return FALSE;
    }

    for(i = 0; i < 256; ++i)
    {
        buf[i] = signature[i];
    }

    modpow3(key, buf);

    /* check pkcs1.5 padding bytes */
    for(i = 0; i < (s32_t)sizeof(padding); ++i)
    {
    	if(buf[i] != padding[i])
    	{
            return FALSE;
        }
    }

    /* check sha digest matches */
    for(; i < 256; ++i)
    {
		if (buf[i] != *sha++)
		{
            return FALSE;
        }
    }

    return TRUE;
}
