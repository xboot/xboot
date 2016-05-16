/*
 * libc/crypto/sha1.c
 */

#include <sha1.h>

#define rol(bits, value)	(((value) << (bits)) | ((value) >> (32 - (bits))))

static void sha1_transform(struct sha1_ctx_t * ctx)
{
	uint32_t W[80];
	uint32_t A, B, C, D, E;
	uint8_t * p = ctx->buf;
	int t;

	for(t = 0; t < 16; ++t)
	{
		uint32_t tmp =  *p++ << 24;
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
		uint32_t tmp = rol(5, A) + E + W[t];

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

void sha1_init(struct sha1_ctx_t * ctx)
{
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xC3D2E1F0;
	ctx->count = 0;
}

void sha1_update(struct sha1_ctx_t * ctx, const void * data, int len)
{
	int i = (int)(ctx->count & 63);
	const uint8_t * p = (const uint8_t *)data;

	ctx->count += len;
	while(len--)
	{
		ctx->buf[i++] = *p++;
		if (i == 64)
		{
			sha1_transform(ctx);
			i = 0;
		}
	}
}

const uint8_t * sha1_final(struct sha1_ctx_t * ctx)
{
	uint8_t * p = ctx->buf;
	uint64_t cnt = ctx->count * 8;
	int i;

	sha1_update(ctx, (uint8_t *)"\x80", 1);
	while((ctx->count & 63) != 56)
	{
		sha1_update(ctx, (uint8_t *)"\0", 1);
	}
	for(i = 0; i < 8; ++i)
	{
		uint8_t tmp = (uint8_t)(cnt >> ((7 - i) * 8));
		sha1_update(ctx, &tmp, 1);
	}

	for(i = 0; i < 5; i++)
		{
		uint32_t tmp = ctx->state[i];
		*p++ = tmp >> 24;
		*p++ = tmp >> 16;
		*p++ = tmp >> 8;
		*p++ = tmp >> 0;
	}

	return ctx->buf;
}

/*
 * Compute sha1 (160-bits) message digest
 */
const uint8_t * sha1_hash(const void * data, int len, uint8_t * digest)
{
	struct sha1_ctx_t ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, data, len);
	memcpy(digest, sha1_final(&ctx), SHA1_DIGEST_SIZE);
	return digest;
}
