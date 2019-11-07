#include <main.h>

enum {
	ZFLAG_LZ4_COMPRESS			= (1 << 0),
	ZFLAG_SHA256_BINDID			= (1 << 1),
	ZFLAG_ECDSA256_SIGNATURE	= (1 << 2),
};

struct zdesc_t {
	uint8_t magic[4];		/* ZBL! */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t signature[64];	/* Ecdsa256 signature */
	uint8_t csize[4];		/* Compress size */
	uint8_t dsize[4];		/* Uncompress size */
	uint8_t public[33];		/* Ecdsa256 public key */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t flag;			/* Zflag */
};

static inline unsigned char hex_to_bin(char c)
{
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static inline unsigned char hex_string(const char * s, int o)
{
	return (hex_to_bin(s[o]) << 4) | hex_to_bin(s[o + 1]);
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mkz [-majoy number] [-minior number] [-patch number] [-z] [-s reserve-size] [-i id] [-p public-key] [-k private-key] <bootloader> <zbootloader>\r\n");
}

int main(int argc, char * argv[])
{
	struct zdesc_t * z;
	struct sha256_ctx_t ctx;
	FILE * blfp, * zblfp;
	char * blbuf, * zblbuf;
	char * blpath = NULL, * zblpath = NULL;
	char * id = NULL;
	char * pub = NULL;
	char * key = NULL;
	uint8_t public[33] = { 0 };
	uint8_t private[32] = { 0 };
	uint8_t majoy = 0;
	uint8_t minior = 0;
	uint8_t patch = 0;
	uint8_t flag = 0;
	int size = 0;
	int bllen, zbllen;
	int clen, len;
	int i, index = 0;
	int o;

	memset(&public[0], 0, 33);
	memset(&private[0], 0, 32);

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-majoy") && (argc > i + 1))
		{
			majoy = (uint8_t)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-minior") && (argc > i + 1))
		{
			minior = (uint8_t)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-patch") && (argc > i + 1))
		{
			patch = (uint8_t)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-z"))
		{
			flag |= ZFLAG_LZ4_COMPRESS;
		}
		else if(!strcmp(argv[i], "-s") && (argc > i + 1))
		{
			size = (int)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-i") && (argc > i + 1))
		{
			id = argv[i + 1];
			if(id && (strcmp(id, "") != 0) && (strlen(key) > 0))
				flag |= ZFLAG_SHA256_BINDID;
			i++;
		}
		else if(!strcmp(argv[i], "-p") && (argc > i + 1))
		{
			pub = argv[i + 1];
			if(pub && (strcmp(pub, "") != 0) && (strlen(pub) == 33 * 2))
			{
				for(o = 0; o < 33; o++)
					public[o] = hex_string(pub, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-k") && (argc > i + 1))
		{
			key = argv[i + 1];
			if(key && (strcmp(key, "") != 0) && (strlen(key) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					private[o] = hex_string(key, o * 2);
				flag |= ZFLAG_ECDSA256_SIGNATURE;
			}
			i++;
		}
		else if(*argv[i] == '-')
		{
			usage();
			return -1;
		}
		else if(*argv[i] != '-' && strcmp(argv[i], "-") != 0)
		{
			if(index == 0)
				blpath = argv[i];
			else if(index == 1)
				zblpath = argv[i];
			else
			{
				usage();
				return -1;
			}
			index++;
		}
	}

	if(!blpath || !zblpath)
	{
		usage();
		return -1;
	}

	blfp = fopen(blpath, "r+b");
	if(blfp == NULL)
	{
		printf("Open bootloader error.\r\n");
		return -1;
	}
	fseek(blfp, 0L, SEEK_END);
	bllen = ftell(blfp);
	fseek(blfp, 0L, SEEK_SET);
	if(size > bllen)
	{
		printf("The reserve size is too large.\r\n");
		fclose(blfp);
		return -1;
	}
	blbuf = malloc(bllen);
	memset(blbuf, 0, bllen);
	if(fread(blbuf, 1, bllen, blfp) != bllen)
	{
		printf("Can't read bootloader.\r\n");
		free(blbuf);
		fclose(blfp);
		return -1;
	}
	fclose(blfp);

	if(flag & ZFLAG_LZ4_COMPRESS)
	{
		len = LZ4_compressBound(bllen);
		zbllen = size + sizeof(struct zdesc_t) + len;
		zblbuf = malloc(zbllen);
		memset(zblbuf, 0, zbllen);
		memcpy(&zblbuf[0], &blbuf[0], size);
		clen = LZ4_compress_HC(&blbuf[0], &zblbuf[size + sizeof(struct zdesc_t)], bllen, len, 12);
		zbllen = size + sizeof(struct zdesc_t) + clen;
	}
	else
	{
		zbllen = size + sizeof(struct zdesc_t) + bllen;
		zblbuf = malloc(zbllen);
		memset(zblbuf, 0, zbllen);
		memcpy(&zblbuf[0], &blbuf[0], size);
		clen = bllen;
		zbllen = size + sizeof(struct zdesc_t) + clen;
	}

	z = (struct zdesc_t *)&zblbuf[size];
	z->magic[0] = 'Z';
	z->magic[1] = 'B';
	z->magic[2] = 'L';
	z->magic[3] = '!';
	z->csize[0] = (clen >> 24) & 0xff;
	z->csize[1] = (clen >> 16) & 0xff;
	z->csize[2] = (clen >>  8) & 0xff;
	z->csize[3] = (clen >>  0) & 0xff;
	z->dsize[0] = (bllen >> 24) & 0xff;
	z->dsize[1] = (bllen >> 16) & 0xff;
	z->dsize[2] = (bllen >>  8) & 0xff;
	z->dsize[3] = (bllen >>  0) & 0xff;
	memcpy(&z->public[0], &public[0], 33);
	z->majoy = majoy;
	z->minior = minior;
	z->patch = patch;
	z->flag = flag;

	sha256_init(&ctx);
	if(z->flag & ZFLAG_SHA256_BINDID)
		sha256_update(&ctx, (void *)(id), strlen(id));
	sha256_update(&ctx, (void *)(&z->csize[0]), 4);
	sha256_update(&ctx, (void *)(&z->dsize[0]), 4);
	sha256_update(&ctx, (void *)(&z->public[0]), 33);
	sha256_update(&ctx, (void *)(&z->majoy), 1);
	sha256_update(&ctx, (void *)(&z->minior), 1);
	sha256_update(&ctx, (void *)(&z->patch), 1);
	sha256_update(&ctx, (void *)(&z->flag), 1);
	sha256_update(&ctx, (void *)(&zblbuf[size + sizeof(struct zdesc_t)]), clen);
	memcpy(&z->sha256[0], sha256_final(&ctx), SHA256_DIGEST_SIZE);
	if(z->flag & ZFLAG_ECDSA256_SIGNATURE)
	{
		ecdsa256_sign(private, &z->sha256[0], &z->signature[0]);
		if(!ecdsa256_verify(&z->public[0], &z->sha256[0], &z->signature[0]))
		{
			printf("Public key:\r\n\t");
			for(o = 0; o < 33; o++)
				printf("%02x", z->public[o]);
			printf("\r\n");

			printf("Private key:\r\n\t");
			for(o = 0; o < 32; o++)
				printf("%02x", private[o]);
			printf("\r\n");

			printf("Signature verify failed, please check the public and private key!\r\n");
			free(zblbuf);
			return -1;
		}
	}

	zblfp = fopen(zblpath, "w+b");
	if(zblfp == NULL)
	{
		printf("Open zbootloader error.\r\n");
		free(zblbuf);
		return -1;
	}
	if(fwrite(zblbuf, 1, zbllen, zblfp) != zbllen)
	{
		printf("Write bootloader error.\r\n");
		free(blbuf);
		free(zblbuf);
		fclose(zblfp);
		return -1;
	}

	free(blbuf);
	free(zblbuf);
	fclose(zblfp);

	printf("Compressed %d bytes into %d bytes ==> %f%% %s%s%s\r\n", bllen, clen, clen * 100.0 / bllen, (flag & ZFLAG_LZ4_COMPRESS) ? "[C]" : "", (flag & ZFLAG_SHA256_BINDID) ? "[I]" : "", (flag & ZFLAG_ECDSA256_SIGNATURE) ? "[S]" : "");
	return 0;
}
