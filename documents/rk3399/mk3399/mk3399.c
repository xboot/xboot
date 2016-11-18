#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct image_header_t {
	uint8_t magic[16];					/* magic */

	uint32_t load_addr;					/* physical load addr */
	uint32_t load_size;					/* size in bytes */
	uint32_t crc32;						/* crc32 */
	uint32_t hash_len;					/* 20 or 32 , 0 is no hash*/
	uint8_t hash[32];					/* sha */

	uint8_t reserved[1024 - 32 - 32];
	uint32_t signtag;					/* 0x4E474953 */
	uint32_t signlen;					/* maybe 128 or 256 */
	uint8_t rsahash[256];				/* maybe 128 or 256, using max size 256 */
	uint8_t reserved2[2048 - 1024 - 256 - 8];
};

struct sha256_ctx_t {
	uint64_t count;
	uint8_t buf[64];
	uint32_t state[8];
};
#define SHA256_DIGEST_SIZE	(32)
#define ror(value, bits)	(((value) >> (bits)) | ((value) << (32 - (bits))))
#define shr(value, bits)	((value) >> (bits))

static const uint32_t K[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_transform(struct sha256_ctx_t * ctx)
{
	uint32_t W[64];
	uint32_t A, B, C, D, E, F, G, H;
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

	for(; t < 64; t++)
	{
		uint32_t s0 = ror(W[t-15], 7) ^ ror(W[t-15], 18) ^ shr(W[t-15], 3);
		uint32_t s1 = ror(W[t-2], 17) ^ ror(W[t-2], 19) ^ shr(W[t-2], 10);
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
		uint32_t s0 = ror(A, 2) ^ ror(A, 13) ^ ror(A, 22);
		uint32_t maj = (A & B) ^ (A & C) ^ (B & C);
		uint32_t t2 = s0 + maj;
		uint32_t s1 = ror(E, 6) ^ ror(E, 11) ^ ror(E, 25);
		uint32_t ch = (E & F) ^ ((~E) & G);
		uint32_t t1 = H + s1 + ch + K[t] + W[t];

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

void sha256_init(struct sha256_ctx_t * ctx)
{
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

static void sha256_update(struct sha256_ctx_t * ctx, const void * data, int len)
{
	int i = (int)(ctx->count & 63);
	const uint8_t * p = (const uint8_t *)data;

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

static const uint8_t * sha256_final(struct sha256_ctx_t * ctx)
{
	uint8_t * p = ctx->buf;
	uint64_t cnt = ctx->count * 8;
	int i;

	sha256_update(ctx, (uint8_t *)"\x80", 1);
	while ((ctx->count & 63) != 56)
	{
		sha256_update(ctx, (uint8_t *)"\0", 1);
	}
	for(i = 0; i < 8; ++i)
	{
		uint8_t tmp = (uint8_t)(cnt >> ((7 - i) * 8));
		sha256_update(ctx, &tmp, 1);
	}

	for(i = 0; i < 8; i++)
	{
		uint32_t tmp = ctx->state[i];
		*p++ = tmp >> 24;
		*p++ = tmp >> 16;
		*p++ = tmp >> 8;
		*p++ = tmp >> 0;
	}

	return ctx->buf;
}

static const uint32_t crc_table[256] = {
	0x00000000, 0x04c10db7, 0x09821b6e, 0x0d4316d9,
	0x130436dc, 0x17c53b6b, 0x1a862db2, 0x1e472005,
	0x26086db8, 0x22c9600f, 0x2f8a76d6, 0x2b4b7b61,
	0x350c5b64, 0x31cd56d3, 0x3c8e400a, 0x384f4dbd,
	0x4c10db70, 0x48d1d6c7, 0x4592c01e, 0x4153cda9,
	0x5f14edac, 0x5bd5e01b, 0x5696f6c2, 0x5257fb75,
	0x6a18b6c8, 0x6ed9bb7f, 0x639aada6, 0x675ba011,
	0x791c8014, 0x7ddd8da3, 0x709e9b7a, 0x745f96cd,
	0x9821b6e0, 0x9ce0bb57, 0x91a3ad8e, 0x9562a039,
	0x8b25803c, 0x8fe48d8b, 0x82a79b52, 0x866696e5,
	0xbe29db58, 0xbae8d6ef, 0xb7abc036, 0xb36acd81,
	0xad2ded84, 0xa9ece033, 0xa4aff6ea, 0xa06efb5d,
	0xd4316d90, 0xd0f06027, 0xddb376fe, 0xd9727b49,
	0xc7355b4c, 0xc3f456fb, 0xceb74022, 0xca764d95,
	0xf2390028, 0xf6f80d9f, 0xfbbb1b46, 0xff7a16f1,
	0xe13d36f4, 0xe5fc3b43, 0xe8bf2d9a, 0xec7e202d,
	0x34826077, 0x30436dc0, 0x3d007b19, 0x39c176ae,
	0x278656ab, 0x23475b1c, 0x2e044dc5, 0x2ac54072,
	0x128a0dcf, 0x164b0078, 0x1b0816a1, 0x1fc91b16,
	0x018e3b13, 0x054f36a4, 0x080c207d, 0x0ccd2dca,
	0x7892bb07, 0x7c53b6b0, 0x7110a069, 0x75d1adde,
	0x6b968ddb, 0x6f57806c, 0x621496b5, 0x66d59b02,
	0x5e9ad6bf, 0x5a5bdb08, 0x5718cdd1, 0x53d9c066,
	0x4d9ee063, 0x495fedd4, 0x441cfb0d, 0x40ddf6ba,
	0xaca3d697, 0xa862db20, 0xa521cdf9, 0xa1e0c04e,
	0xbfa7e04b, 0xbb66edfc, 0xb625fb25, 0xb2e4f692,
	0x8aabbb2f, 0x8e6ab698, 0x8329a041, 0x87e8adf6,
	0x99af8df3, 0x9d6e8044, 0x902d969d, 0x94ec9b2a,
	0xe0b30de7, 0xe4720050, 0xe9311689, 0xedf01b3e,
	0xf3b73b3b, 0xf776368c, 0xfa352055, 0xfef42de2,
	0xc6bb605f, 0xc27a6de8, 0xcf397b31, 0xcbf87686,
	0xd5bf5683, 0xd17e5b34, 0xdc3d4ded, 0xd8fc405a,
	0x6904c0ee, 0x6dc5cd59, 0x6086db80, 0x6447d637,
	0x7a00f632, 0x7ec1fb85, 0x7382ed5c, 0x7743e0eb,
	0x4f0cad56, 0x4bcda0e1, 0x468eb638, 0x424fbb8f,
	0x5c089b8a, 0x58c9963d, 0x558a80e4, 0x514b8d53,
	0x25141b9e, 0x21d51629, 0x2c9600f0, 0x28570d47,
	0x36102d42, 0x32d120f5, 0x3f92362c, 0x3b533b9b,
	0x031c7626, 0x07dd7b91, 0x0a9e6d48, 0x0e5f60ff,
	0x101840fa, 0x14d94d4d, 0x199a5b94, 0x1d5b5623,
	0xf125760e, 0xf5e47bb9, 0xf8a76d60, 0xfc6660d7,
	0xe22140d2, 0xe6e04d65, 0xeba35bbc, 0xef62560b,
	0xd72d1bb6, 0xd3ec1601, 0xdeaf00d8, 0xda6e0d6f,
	0xc4292d6a, 0xc0e820dd, 0xcdab3604, 0xc96a3bb3,
	0xbd35ad7e, 0xb9f4a0c9, 0xb4b7b610, 0xb076bba7,
	0xae319ba2, 0xaaf09615, 0xa7b380cc, 0xa3728d7b,
	0x9b3dc0c6, 0x9ffccd71, 0x92bfdba8, 0x967ed61f,
	0x8839f61a, 0x8cf8fbad, 0x81bbed74, 0x857ae0c3,
	0x5d86a099, 0x5947ad2e, 0x5404bbf7, 0x50c5b640,
	0x4e829645, 0x4a439bf2, 0x47008d2b, 0x43c1809c,
	0x7b8ecd21, 0x7f4fc096, 0x720cd64f, 0x76cddbf8,
	0x688afbfd, 0x6c4bf64a, 0x6108e093, 0x65c9ed24,
	0x11967be9, 0x1557765e, 0x18146087, 0x1cd56d30,
	0x02924d35, 0x06534082, 0x0b10565b, 0x0fd15bec,
	0x379e1651, 0x335f1be6, 0x3e1c0d3f, 0x3add0088,
	0x249a208d, 0x205b2d3a, 0x2d183be3, 0x29d93654,
	0xc5a71679, 0xc1661bce, 0xcc250d17, 0xc8e400a0,
	0xd6a320a5, 0xd2622d12, 0xdf213bcb, 0xdbe0367c,
	0xe3af7bc1, 0xe76e7676, 0xea2d60af, 0xeeec6d18,
	0xf0ab4d1d, 0xf46a40aa, 0xf9295673, 0xfde85bc4,
	0x89b7cd09, 0x8d76c0be, 0x8035d667, 0x84f4dbd0,
	0x9ab3fbd5, 0x9e72f662, 0x9331e0bb, 0x97f0ed0c,
	0xafbfa0b1, 0xab7ead06, 0xa63dbbdf, 0xa2fcb668,
	0xbcbb966d, 0xb87a9bda, 0xb5398d03, 0xb1f880b4,
};

static uint32_t crc32(uint32_t crc, const uint8_t * buf, size_t len)
{
	do {
		crc = crc_table[((crc >> 24) ^ (*buf++)) & 255] ^ (crc << 8);
	} while (--len);
	return crc;
}

int main(int argc, char *argv[])
{
	struct image_header_t hdr;
	struct sha256_ctx_t ctx;
	FILE * fi, * fo;
	char * buffer;
	int buflen = 64 * 1024 * 1024;
	int ilen;

	if(argc != 3)
	{
		printf("Usage: mk3399 <input file> <output file>\n");
		return -1;
	}

	buffer = malloc(buflen);
	memset(buffer, 0, buflen);

	fi = fopen(argv[1], "r+b");
	if(!fi)
	{
		printf("Open file '%s' error\n", argv[1]);
		free(buffer);
		return -1;
	}

	fo = fopen(argv[2], "w+b");
	if(!fo)
	{
		printf("Open file '%s' error\n", argv[2]);
		free(buffer);
		close(fi);
		return -1;
	}

	fseek(fi, 0, SEEK_END);
	ilen = ftell(fi);
	fseek(fi, 0, SEEK_SET);

	if(ilen > buflen - sizeof(struct image_header_t))
	{
		printf("The input file too large\n");
		close(fi);
		close(fo);
		free(buffer);
		return -1;
	}

	memset(&hdr, 0, sizeof(struct image_header_t));
	strcpy((char *)hdr.magic, "LOADER  ");
	hdr.load_addr = 0x00200000;

	if(fread(buffer + sizeof(struct image_header_t), 1, ilen, fi) != ilen)
	{
		printf("Read input file error\n");
		close(fi);
		close(fo);
		free(buffer);
		return -1;
	}

	/* Aligned size to 4-byte, rockchip hardware crypto need 4-byte align */
	ilen = (((ilen + 3) >> 2 ) << 2);
	hdr.load_size = ilen;
	hdr.crc32 = crc32(0, (const uint8_t *)buffer + sizeof(struct image_header_t), ilen);

	hdr.hash_len = SHA256_DIGEST_SIZE;
	sha256_init(&ctx);
	sha256_update(&ctx, (void *)buffer + sizeof(struct image_header_t), ilen);
	sha256_update(&ctx, (void *)&hdr.load_addr, sizeof(hdr.load_addr));
	sha256_update(&ctx, (void *)&hdr.load_size, sizeof(hdr.load_size));
	sha256_update(&ctx, (void *)&hdr.hash_len, sizeof(hdr.hash_len));
	memcpy(hdr.hash, sha256_final(&ctx), SHA256_DIGEST_SIZE);
	memcpy(buffer, &hdr, sizeof(struct image_header_t));

	if(fwrite(buffer, ilen + sizeof(struct image_header_t), 1, fo) != 1)
	{
		printf("Write output file error\n");
		close(fi);
		close(fo);
		free(buffer);
		return -1;
	}
	close(fi);
	close(fo);
	free(buffer);

	printf("Generate output file '%s' successed\n", argv[2]);
	return 0;
}
