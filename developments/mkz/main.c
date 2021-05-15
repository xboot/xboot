#include <main.h>

enum {
	ZFLAG_LZ4_COMPRESS			= (1 << 0),
	ZFLAG_AES256_ENCRYPT		= (1 << 1),
	ZFLAG_SHA256_BINDID			= (1 << 2),
	ZFLAG_ECDSA256_SIGNATURE	= (1 << 3),
};

struct zdesc_t {			/* Total 256 bytes */
	uint8_t magic[4];		/* ZBL! */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t signature[64];	/* Ecdsa256 signature of sha256 */
	uint8_t csize[4];		/* Compress size */
	uint8_t dsize[4];		/* Uncompress size */
	uint8_t key[32];		/* Aes256 encrypt key */
	uint8_t public[33];		/* Ecdsa256 public key */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t flag;			/* Zflag */
	uint8_t message[79];	/* Message additionally */
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
	printf("    mkz [-majoy number] [-minior number] [-patch number] [-r reserve-image-size] [-c] [-b bind-id] [-e aes256-encrypt-key] [-p ecdsa256-public-key] [-k ecdsa256-private-key] [-m message] <bootloader> <zbootloader>\r\n");
	printf("    -majoy  The majoy version\r\n");
	printf("    -minior The minior version\r\n");
	printf("    -patch  The patch version\r\n");
	printf("    -r      The reserve size for image\r\n");
	printf("    -c      The lz4 compress flag\r\n");
	printf("    -b      The sha256 hash with id\r\n");
	printf("    -e      The aes256 encrypt key\r\n");
	printf("    -p      The ecdsa256 public key\r\n");
	printf("    -k      The ecdsa256 private key\r\n");
	printf("    -m      The additional message\r\n");
}

int main(int argc, char * argv[])
{
	struct zdesc_t * z;
	struct aes256_ctx_t aesctx;
	struct sha256_ctx_t shactx;
	FILE * blfp, * zblfp;
	char * blbuf, * zblbuf;
	char * blpath = NULL, * zblpath = NULL;
	char * id = NULL;
	char * msg = NULL;
	char * p = NULL;
	uint8_t key[32] = { 0 };
	uint8_t public[33] = { 0 };
	uint8_t private[32] = { 0 };
	uint8_t majoy = 0;
	uint8_t minior = 0;
	uint8_t patch = 0;
	uint8_t flag = 0;
	int rsize = 0;
	int bllen, zbllen;
	int clen, len;
	int i, index = 0;
	int o;

	memset(&key[0], 0, 32);
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
		else if(!strcmp(argv[i], "-r") && (argc > i + 1))
		{
			rsize = (int)strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "-c"))
		{
			flag |= ZFLAG_LZ4_COMPRESS;
		}
		else if(!strcmp(argv[i], "-b") && (argc > i + 1))
		{
			p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
			{
				id = p;
				flag |= ZFLAG_SHA256_BINDID;
			}
			i++;
		}
		else if(!strcmp(argv[i], "-e") && (argc > i + 1))
		{
			p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					key[o] = hex_string(p, o * 2);
				flag |= ZFLAG_AES256_ENCRYPT;
			}
			i++;
		}
		else if(!strcmp(argv[i], "-p") && (argc > i + 1))
		{
			p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 33 * 2))
			{
				for(o = 0; o < 33; o++)
					public[o] = hex_string(p, o * 2);
			}
			i++;
		}
		else if(!strcmp(argv[i], "-k") && (argc > i + 1))
		{
			p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == 32 * 2))
			{
				for(o = 0; o < 32; o++)
					private[o] = hex_string(p, o * 2);
				flag |= ZFLAG_ECDSA256_SIGNATURE;
			}
			i++;
		}
		else if(!strcmp(argv[i], "-m") && (argc > i + 1))
		{
			p = argv[i + 1];
			if(p && (strcmp(p, "") != 0) && (strlen(p) > 0))
				msg = p;
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
	if(rsize > bllen)
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
		zbllen = rsize + sizeof(struct zdesc_t) + len;
		zblbuf = malloc(zbllen);
		memset(zblbuf, 0, zbllen);
		memcpy(&zblbuf[0], &blbuf[0], rsize);
		clen = LZ4_compress_HC(&blbuf[0], &zblbuf[rsize + sizeof(struct zdesc_t)], bllen, len, 12);
		zbllen = rsize + sizeof(struct zdesc_t) + clen;
	}
	else
	{
		zbllen = rsize + sizeof(struct zdesc_t) + bllen;
		zblbuf = malloc(zbllen);
		memset(zblbuf, 0, zbllen);
		memcpy(&zblbuf[0], &blbuf[0], rsize);
		memcpy(&zblbuf[rsize + sizeof(struct zdesc_t)], &blbuf[0], bllen);
		clen = bllen;
		zbllen = rsize + sizeof(struct zdesc_t) + clen;
	}

	if(flag & ZFLAG_AES256_ENCRYPT)
	{
		printf("Aes256 encrypt key:\r\n\t");
		for(o = 0; o < 32; o++)
			printf("%02x", key[o]);
		printf("\r\n");

		aes256_set_key(&aesctx, key);
		aes256_ctr_encrypt(&aesctx, 0, (uint8_t *)&zblbuf[rsize + sizeof(struct zdesc_t)], (uint8_t *)&zblbuf[rsize + sizeof(struct zdesc_t)], clen);
	}

	z = (struct zdesc_t *)&zblbuf[rsize];
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
	memcpy(&z->key[0], &key[0], 32);
	memcpy(&z->public[0], &public[0], 33);
	z->majoy = majoy;
	z->minior = minior;
	z->patch = patch;
	z->flag = flag;
	if(msg)
		strncpy((char *)&z->message[0], msg, 79 - 1);

	sha256_init(&shactx);
	if(z->flag & ZFLAG_SHA256_BINDID)
		sha256_update(&shactx, (void *)(id), strlen(id));
	sha256_update(&shactx, (void *)(&z->csize[0]), 4);
	sha256_update(&shactx, (void *)(&z->dsize[0]), 4);
	sha256_update(&shactx, (void *)(&z->key[0]), 32);
	sha256_update(&shactx, (void *)(&z->public[0]), 33);
	sha256_update(&shactx, (void *)(&z->majoy), 1);
	sha256_update(&shactx, (void *)(&z->minior), 1);
	sha256_update(&shactx, (void *)(&z->patch), 1);
	sha256_update(&shactx, (void *)(&z->flag), 1);
	sha256_update(&shactx, (void *)(&z->message), 79);
	sha256_update(&shactx, (void *)(&zblbuf[rsize + sizeof(struct zdesc_t)]), clen);
	memcpy(&z->sha256[0], sha256_final(&shactx), SHA256_DIGEST_SIZE);
	if(z->flag & ZFLAG_ECDSA256_SIGNATURE)
	{
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

	printf("Compressed %d bytes into %d bytes ==> %f%% %s%s%s%s\r\n", bllen, clen, clen * 100.0 / bllen, (flag & ZFLAG_LZ4_COMPRESS) ? "[C]" : "", (flag & ZFLAG_AES256_ENCRYPT) ? "[E]" : "", (flag & ZFLAG_SHA256_BINDID) ? "[B]" : "", (flag & ZFLAG_ECDSA256_SIGNATURE) ? "[S]" : "");
	return 0;
}
