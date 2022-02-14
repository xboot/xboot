/*
 * sys-hash.c
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

#define ror(v, n)	(((v) >> (n)) | ((v) << (32 - (n))))
#define shr(v, n)	((v) >> (n))

struct sha256_ctx_t {
	uint32_t K[64];
	uint64_t count;
	uint8_t buf[64];
	uint32_t state[8];
};

static void sha256_transform(struct sha256_ctx_t * ctx)
{
	uint32_t W[64];
	uint32_t A, B, C, D, E, F, G, H;
	uint32_t s0, s1, t1, t2, ch, maj;
	uint32_t tmp;
	uint32_t * K;
	uint8_t * p;
	int t;

	K = ctx->K;
	p = ctx->buf;
	for(t = 0; t < 16; ++t)
	{
		tmp =  *p++ << 24;
		tmp |= *p++ << 16;
		tmp |= *p++ << 8;
		tmp |= *p++;
		W[t] = tmp;
	}
	for(; t < 64; t++)
	{
		s0 = ror(W[t-15], 7) ^ ror(W[t-15], 18) ^ shr(W[t-15], 3);
		s1 = ror(W[t-2], 17) ^ ror(W[t-2], 19) ^ shr(W[t-2], 10);
		W[t] = W[t-16] + s0 + W[t-7] + s1;
	}
	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];
	F = ctx->state[5];
	G = ctx->state[6];
	H = ctx->state[7];
	for(t = 0; t < 64; t++)
	{
		s0 = ror(A, 2) ^ ror(A, 13) ^ ror(A, 22);
		maj = (A & B) ^ (A & C) ^ (B & C);
		t2 = s0 + maj;
		s1 = ror(E, 6) ^ ror(E, 11) ^ ror(E, 25);
		ch = (E & F) ^ ((~E) & G);
		t1 = H + s1 + ch + K[t] + W[t];
		H = G;
		G = F;
		F = E;
		E = D + t1;
		D = C;
		C = B;
		B = A;
		A = t1 + t2;
	}
	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
	ctx->state[5] += F;
	ctx->state[6] += G;
	ctx->state[7] += H;
}

static void sha256_init(struct sha256_ctx_t * ctx)
{
	uint32_t * K;

	K = ctx->K;
	K[ 0] = 0x428a2f98; K[ 1] = 0x71374491; K[ 2] = 0xb5c0fbcf; K[ 3] = 0xe9b5dba5;
	K[ 4] = 0x3956c25b; K[ 5] = 0x59f111f1; K[ 6] = 0x923f82a4; K[ 7] = 0xab1c5ed5;
	K[ 8] = 0xd807aa98; K[ 9] = 0x12835b01; K[10] = 0x243185be; K[11] = 0x550c7dc3;
	K[12] = 0x72be5d74; K[13] = 0x80deb1fe; K[14] = 0x9bdc06a7; K[15] = 0xc19bf174;
	K[16] = 0xe49b69c1; K[17] = 0xefbe4786; K[18] = 0x0fc19dc6; K[19] = 0x240ca1cc;
	K[20] = 0x2de92c6f; K[21] = 0x4a7484aa; K[22] = 0x5cb0a9dc; K[23] = 0x76f988da;
	K[24] = 0x983e5152; K[25] = 0xa831c66d; K[26] = 0xb00327c8; K[27] = 0xbf597fc7;
	K[28] = 0xc6e00bf3; K[29] = 0xd5a79147; K[30] = 0x06ca6351; K[31] = 0x14292967;
	K[32] = 0x27b70a85; K[33] = 0x2e1b2138; K[34] = 0x4d2c6dfc; K[35] = 0x53380d13;
	K[36] = 0x650a7354; K[37] = 0x766a0abb; K[38] = 0x81c2c92e; K[39] = 0x92722c85;
	K[40] = 0xa2bfe8a1; K[41] = 0xa81a664b; K[42] = 0xc24b8b70; K[43] = 0xc76c51a3;
	K[44] = 0xd192e819; K[45] = 0xd6990624; K[46] = 0xf40e3585; K[47] = 0x106aa070;
	K[48] = 0x19a4c116; K[49] = 0x1e376c08; K[50] = 0x2748774c; K[51] = 0x34b0bcb5;
	K[52] = 0x391c0cb3; K[53] = 0x4ed8aa4a; K[54] = 0x5b9cca4f; K[55] = 0x682e6ff3;
	K[56] = 0x748f82ee; K[57] = 0x78a5636f; K[58] = 0x84c87814; K[59] = 0x8cc70208;
	K[60] = 0x90befffa; K[61] = 0xa4506ceb; K[62] = 0xbef9a3f7; K[63] = 0xc67178f2;

	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
	ctx->count = 0;
}

static void sha256_update(struct sha256_ctx_t * ctx, char * buf, int len)
{
	uint8_t * p;
	int i;

	p = (uint8_t *)buf;
	i = (int)(ctx->count & 63);
	ctx->count += len;
	while(len--)
	{
		ctx->buf[i++] = *p++;
		if(i == 64)
		{
			sha256_transform(ctx);
			i = 0;
		}
	}
}

static void sha256_final(struct sha256_ctx_t * ctx)
{
	uint64_t cnt;
	uint32_t tmp;
	uint8_t * p;
	char c80, c0, c;
	int i;

	cnt = ctx->count << 3;
	p = ctx->buf;
	c80 = 0x80;
	c0 = 0x0;
	sha256_update(ctx, &c80, 1);
	while((ctx->count & 63) != 56)
		sha256_update(ctx, &c0, 1);
	for(i = 0; i < 8; ++i)
	{
		c = (uint8_t)(cnt >> ((7 - i) << 3));
		sha256_update(ctx, &c, 1);
	}
	for(i = 0; i < 8; i++)
	{
		tmp = ctx->state[i];
		*p++ = tmp >> 24;
		*p++ = tmp >> 16;
		*p++ = tmp >> 8;
		*p++ = tmp >> 0;
	}
}

int sys_hash(char * buf, int len, char * sha256)
{
	struct sha256_ctx_t ctx;
	int i;

	sha256_init(&ctx);
	sha256_update(&ctx, buf, len);
	sha256_final(&ctx);
	for(i = 0; i < 32; i++)
	{
		if(ctx.buf[i] != sha256[i])
			return 0;
	}
	return 1;
}
