#include <main.h>

struct zdesc_t {			/* Total 256 bytes */
	uint8_t magic[4];		/* ZB??, I for bind id, E for encrypt image */
	uint8_t key[32];		/* Aes256 encrypt key (hardcode or efuse suggested) */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t signature[64];	/* Ecdsa256 signature of sha256 */
	uint8_t csize[4];		/* Compress size of image */
	uint8_t dsize[4];		/* Decompress size of image */
	uint8_t public[33];		/* Ecdsa256 public key (hardcode suggested) */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t message[80];	/* Message additionally */
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mkz [-majoy number] [-minior number] [-patch number] [-r reserve-image-size] [-k aes256-encrypt-key] [-pb ecdsa256-public-key] [-pv ecdsa256-private-key] [-m message] [-g uniqueid] [-i uniqueid] [-e] <image> <zimage>\r\n");
	printf("    -majoy  The majoy version\r\n");
	printf("    -minior The minior version\r\n");
	printf("    -patch  The patch version\r\n");
	printf("    -r      The reserve size\r\n");
	printf("    -k      The aes256 encrypt key\r\n");
	printf("    -pb     The ecdsa256 public key\r\n");
	printf("    -pv     The ecdsa256 private key\r\n");
	printf("    -m      The additional message\r\n");
	printf("    -g      Generate aes256 encrypt key by uniqueid\r\n");
	printf("    -i      Enable sha256 hash with uniqueid\r\n");
	printf("    -e      Enable aes256 encrypt image\r\n");
}

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

int main(int argc, char * argv[])
{
	struct aes256_ctx_t aesctx;
	struct sha256_ctx_t shactx;
	struct zdesc_t * z;
	FILE * blfp, * zblfp;
	char * blpath = NULL;
	char * zblpath = NULL;
	char * blbuf = NULL;
	char * zblbuf = NULL;
	char * msg = NULL;
	uint8_t key[] = {
		0x67, 0x94, 0x08, 0xdc, 0x82, 0xae, 0x80, 0xd4,
		0x11, 0xd5, 0xd9, 0x72, 0x0b, 0x65, 0xa4, 0x3f,
		0xc4, 0xf1, 0x53, 0x4f, 0xa5, 0x63, 0xfb, 0x28,
		0xc6, 0xcd, 0x89, 0x28, 0xe4, 0x6a, 0xaa, 0xe9,
	};
	uint8_t public[33] = {
		0x03, 0xcf, 0xd1, 0x8e, 0x4a, 0x4b, 0x40, 0xd6,
		0x52, 0x94, 0x48, 0xaa, 0x2d, 0xf8, 0xbb, 0xb6,
		0x77, 0x12, 0x82, 0x58, 0xb8, 0xfb, 0xfc, 0x5b,
		0x9e, 0x49, 0x2f, 0xbb, 0xba, 0x4e, 0x84, 0x83,
		0x2f,
	};
	uint8_t private[32] = {
		0xdc, 0x57, 0xb8, 0xa9, 0xe0, 0xe2, 0xb7, 0xf8,
		0xb4, 0xc9, 0x29, 0xbd, 0x8d, 0xb2, 0x84, 0x4e,
		0x53, 0xf0, 0x1f, 0x17, 0x1b, 0xbc, 0xdf, 0x6e,
		0x62, 0x89, 0x08, 0xdb, 0xf2, 0xb2, 0xe6, 0xa9,
	};
	uint8_t majoy = 0, minior = 0, patch = 0;
	char * keygen = NULL;
	char * uniqueid = NULL;
	int rsize = 0, encrypt = 0;
	int index = 0;
	int bllen, zbllen;
	int clen, len;
	int i, o;

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
		else if(!strcmp(argv[i], "-r") && (argc > i + 1))
		{
			rsize = (int)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-k") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					key[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-pb") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 33 * 2))
			{
				for(o = 0; o < 33; o++)
					public[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-pv") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					private[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-m") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
				msg = p;
			i++;
		}
		else if(!strcmp(argv[i], "-g") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
				keygen = p;
			i++;
		}
		else if(!strcmp(argv[i], "-i") && (argc > i + 1))
		{
			char * p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
				uniqueid = p;
			i++;
		}
		else if(!strcmp(argv[i], "-e"))
		{
			encrypt = 1;
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
	if(keygen)
		sha256_hash(keygen, strlen(keygen), key);
	blfp = fopen(blpath, "r+b");
	if(blfp == NULL)
	{
		printf("Open image error\r\n");
		return -1;
	}
	fseek(blfp, 0L, SEEK_END);
	bllen = ftell(blfp);
	fseek(blfp, 0L, SEEK_SET);
	if(rsize > bllen)
	{
		printf("The reserve size is too large\r\n");
		fclose(blfp);
		return -1;
	}
	blbuf = malloc(bllen);
	memset(blbuf, 0, bllen);
	if(fread(blbuf, 1, bllen, blfp) != bllen)
	{
		printf("Can't read image\r\n");
		free(blbuf);
		fclose(blfp);
		return -1;
	}
	fclose(blfp);

	len = LZ4_compressBound(bllen);
	zbllen = rsize + sizeof(struct zdesc_t) + len;
	zblbuf = malloc(zbllen);
	memset(zblbuf, 0, zbllen);
	memcpy(&zblbuf[0], &blbuf[0], rsize);
	clen = LZ4_compress_HC(&blbuf[0], &zblbuf[rsize + sizeof(struct zdesc_t)], bllen, len, 12);
	zbllen = rsize + sizeof(struct zdesc_t) + clen;
	if(encrypt)
	{
		aes256_set_key(&aesctx, key);
		aes256_ctr_encrypt(&aesctx, 0, (uint8_t *)&zblbuf[rsize + sizeof(struct zdesc_t)], (uint8_t *)&zblbuf[rsize + sizeof(struct zdesc_t)], clen);
	}

	z = (struct zdesc_t *)&zblbuf[rsize];
	z->magic[0] = 'Z';
	z->magic[1] = 'B';
	z->magic[2] = uniqueid ? 'I' : 0;
	z->magic[3] = encrypt ? 'E' : 0;
	if(keygen)
		memset(&z->key[0], 0, 32);
	else
		memcpy(&z->key[0], &key[0], 32);
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
	if(msg)
		strncpy((char *)&z->message[0], msg, 80 - 1);

	sha256_init(&shactx);
	if(uniqueid)
		sha256_update(&shactx, (void *)(uniqueid), strlen(uniqueid));
	sha256_update(&shactx, (void *)(&z->csize[0]), 4);
	sha256_update(&shactx, (void *)(&z->dsize[0]), 4);
	sha256_update(&shactx, (void *)(&z->public[0]), 33);
	sha256_update(&shactx, (void *)(&z->majoy), 1);
	sha256_update(&shactx, (void *)(&z->minior), 1);
	sha256_update(&shactx, (void *)(&z->patch), 1);
	sha256_update(&shactx, (void *)(&z->message[0]), 80);
	sha256_update(&shactx, (void *)(&zblbuf[rsize + sizeof(struct zdesc_t)]), clen);
	memcpy(&z->sha256[0], sha256_final(&shactx), SHA256_DIGEST_SIZE);

	printf("Aes256 encrypt key:\r\n\t");
	for(o = 0; o < 32; o++)
		printf("%02x", key[o]);
	printf("\r\n");
	printf("Ecdsa256 public key:\r\n\t");
	for(o = 0; o < 33; o++)
		printf("%02x", z->public[o]);
	printf("\r\n");
	printf("Ecdsa256 private key:\r\n\t");
	for(o = 0; o < 32; o++)
		printf("%02x", private[o]);
	printf("\r\n");

	ecdsa256_sign(private, &z->sha256[0], &z->signature[0]);
	if(!ecdsa256_verify(&z->public[0], &z->sha256[0], &z->signature[0]))
	{
		printf("Ecdsa256 signature verify failed, please check the ecdsa256 public and private key!\r\n");
		free(zblbuf);
		return -1;
	}
	aes256_set_key(&aesctx, key);
	aes256_ctr_encrypt(&aesctx, 0, (uint8_t *)&zblbuf[rsize + 36], (uint8_t *)&zblbuf[rsize + 36], sizeof(struct zdesc_t) - 36);

	zblfp = fopen(zblpath, "w+b");
	if(zblfp == NULL)
	{
		printf("Open zimage error\r\n");
		free(zblbuf);
		return -1;
	}
	if(fwrite(zblbuf, 1, zbllen, zblfp) != zbllen)
	{
		printf("Write zimage error\r\n");
		free(blbuf);
		free(zblbuf);
		fclose(zblfp);
		return -1;
	}
	free(blbuf);
	free(zblbuf);
	fclose(zblfp);

	printf("Compressed %d bytes into %d bytes ==> %f%% %s%s%s\r\n", bllen, clen, clen * 100.0 / bllen, keygen ? "[G]" : "", uniqueid ? "[I]" : "", encrypt ? "[E]" : "");
	return 0;
}
