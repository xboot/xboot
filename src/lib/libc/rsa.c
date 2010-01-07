/*
 * lib/libc/rsa.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <rsa.h>

/*
 * a[] -= mod
 */
static void subM(struct rsa_public_key * key, x_u32 * a)
{
	x_s64 A = 0;
    x_s32 i;

    for(i = 0; i < key->len; ++i)
    {
        A += (x_u64)a[i] - key->n[i];
        a[i] = (x_u32)A;
        A >>= 32;
    }
}

/*
 * return a[] >= mod
 */
static x_s32 geM(struct rsa_public_key * key, const x_u32 * a)
{
	x_s32 i;

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
static void montMulAdd(struct rsa_public_key * key, x_u32 * c, const x_u32 a, const x_u32 * b)
{
	x_u64 A = (x_u64)a * b[0] + c[0];
    x_u32 d0 = (x_u32)A * key->n0inv;
    x_u64 B = (x_u64)d0 * key->n[0] + (x_u32)A;
    x_s32 i;

    for(i = 1; i < key->len; ++i)
    {
        A = (A >> 32) + (x_u64)a * b[i] + c[i];
        B = (B >> 32) + (x_u64)d0 * key->n[i] + (x_u32)A;
        c[i - 1] = (x_u32)B;
    }

    A = (A >> 32) + (B >> 32);

    c[i - 1] = (x_u32)A;

    if(A >> 32)
    {
        subM(key, c);
    }
}

/*
 * montgomery c[] = a[] * b[] / R % mod
 */
static void montMul(struct rsa_public_key * key, x_u32 * c, const x_u32 * a, const x_u32 * b)
{
	x_s32 i;

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
static void modpow3(struct rsa_public_key * key, x_u8 * inout)
{
    x_u32 a[256/sizeof(x_u32)];
    x_u32 aR[256/sizeof(x_u32)];
    x_u32 aaR[256/sizeof(x_u32)];
    x_u32 *aaa = aR;
    x_s32 i;

    /*
     * convert from big endian byte array to little endian word array.
     */
    for(i = 0; i < key->len; ++i)
    {
		x_u32 tmp = (inout[((key->len - 1 - i) * 4) + 0] << 24) |	\
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
    	x_u32 tmp = aaa[i];
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
static const x_u8 padding[256 - 20] = {
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
x_bool rsa_verify(struct rsa_public_key * key, const x_u8 * signature, const x_s32 len, const x_u8 * sha)
{
    x_u8 buf[256];
    x_s32 i;

    if(key->len != (256/sizeof(x_u32)))
    {
    	/* wrong key passed in */
        return FALSE;
    }

    if(len != sizeof(buf))
    {
    	/* wrong input length */
    	return FALSE;
    }

    for(i = 0; i < len; ++i)
    {
        buf[i] = signature[i];
    }

    modpow3(key, buf);

    /* check pkcs1.5 padding bytes */
    for(i = 0; i < (x_s32)sizeof(padding); ++i)
    {
    	if(buf[i] != padding[i])
    	{
            return FALSE;
        }
    }

    /* check sha digest matches */
    for(; i < len; ++i)
    {
		if (buf[i] != *sha++)
		{
            return FALSE;
        }
    }

    return TRUE;
}
