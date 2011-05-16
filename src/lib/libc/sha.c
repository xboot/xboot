/*
 * lib/libc/sha.c
 *
 * secure hash algorithm
 */

#include <xboot.h>
#include <types.h>
#include <sha.h>


struct sha_ctx {
	u64_t count;
	u8_t buf[64];
	u32_t state[5];
};

#define rol(bits, value)	(((value) << (bits)) | ((value) >> (32 - (bits))))

static void sha1_transform(struct sha_ctx * ctx)
{
	u32_t W[80];
    u32_t A, B, C, D, E;
    u8_t * p = ctx->buf;
    u32_t tmp;
    s32_t t;

    for(t = 0; t < 16; ++t)
    {
        tmp =  *p++ << 24;
        tmp |= *p++ << 16;
        tmp |= *p++ << 8;
        tmp |= *p++;
        W[t] = tmp;
    }

    for(; t < 80; t++)
    {
    	W[t] = rol(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

    for(t = 0; t < 80; t++)
    {
    	tmp = rol(5, A) + E + W[t];

        if (t < 20)
            tmp += (D ^ (B & (C ^ D))) + 0x5A827999;
        else if ( t < 40)
            tmp += (B ^ C ^ D) + 0x6ED9EBA1;
        else if ( t < 60)
            tmp += ((B & C) | (D & (B | C))) + 0x8F1BBCDC;
        else
            tmp += (B ^ C ^ D) + 0xCA62C1D6;

        E = D;
        D = C;
        C = rol(30, B);
        B = A;
        A = tmp;
    }

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
}

static void sha_init(struct sha_ctx * ctx)
{
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xC3D2E1F0;
	ctx->count = 0;
}

static void sha_update(struct sha_ctx * ctx, const void * data, s32_t len)
{
	s32_t i = ctx->count % sizeof(ctx->buf);
	const u8_t * p = (const u8_t *)data;

	ctx->count += len;
	while(len--)
	{
		ctx->buf[i++] = *p++;
		if(i == sizeof(ctx->buf))
		{
			sha1_transform(ctx);
			i = 0;
		}
	}
}

static const u8_t * sha_final(struct sha_ctx * ctx)
{
	u8_t * p = ctx->buf;
	u64_t cnt = ctx->count * 8;
	u32_t i, j;
	u8_t c;

	sha_update(ctx, (u8_t *)"\x80", 1);
	while((ctx->count % sizeof(ctx->buf)) != (sizeof(ctx->buf) - 8))
	{
		sha_update(ctx, (u8_t *)"\0", 1);
	}

	for(i = 0; i < 8; ++i)
    {
		c = cnt >> ((7 - i) * 8);
        sha_update(ctx, &c, 1);
    }

	for(i = 0; i < 5; i++)
	{
    	j = ctx->state[i];
        *p++ = j >> 24;
        *p++ = j >> 16;
        *p++ = j >> 8;
        *p++ = j >> 0;
	}

	return ctx->buf;
}

/*
 * compute SHA1 (160-bit) message digest
 *
 * the sums are computed as described in FIPS-180-1
 */
const u8_t * sha1(const void * data, s32_t len, u8_t * digest)
{
	const u8_t * p;
	struct sha_ctx ctx;
	s32_t i;

	sha_init(&ctx);
	sha_update(&ctx, data, len);
	p = sha_final(&ctx);

	for(i = 0; i < 20; ++i)
	{
		digest[i] = *p++;
	}

	return digest;
}
