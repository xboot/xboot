#include <main.h>

struct zdesc_t {
	uint8_t magic[4];
	uint8_t crc[4];
	uint8_t ssize[4];
	uint8_t dsize[4];
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mkz <keep-header-length> <bootloader> <zbootloader>\r\n");
}

int main(int argc, char * argv[])
{
	struct zdesc_t * z;
	FILE * blfp, * zblfp;
	char * blbuf, * zblbuf;
	char * blpath = NULL, * zblpath = NULL;
	int bllen, zbllen;
	int actlen, maxlen;
	int length = 0;
	uint32_t crc = 0;

	if(argc != 4)
	{
		usage();
		return -1;
	}

	length = (int)strtoul(argv[1], NULL, 0);
	blpath = argv[2];
	zblpath = argv[3];

	if(!blpath || !zblpath)
	{
		usage();
		return -1;
	}

	blfp = fopen(blpath, "r+b");
	if(blfp == NULL)
	{
		printf("Open bootloader error\r\n");
		return -1;
	}
	fseek(blfp, 0L, SEEK_END);
	bllen = ftell(blfp);
	fseek(blfp, 0L, SEEK_SET);
	if(length > bllen)
	{
		printf("Keep header length too large\r\n");
		fclose(blfp);
		return -1;
	}
	blbuf = malloc(bllen);
	memset(blbuf, 0, bllen);
	if(fread(blbuf, 1, bllen, blfp) != bllen)
	{
		printf("Can't read bootloader\r\n");
		free(blbuf);
		fclose(blfp);
		return -1;
	}
	fclose(blfp);

	maxlen = LZ4_compressBound(bllen);
	zbllen = length + sizeof(struct zdesc_t) + maxlen;
	zblbuf = malloc(zbllen);
	memset(zblbuf, 0, zbllen);
	memcpy(&zblbuf[0], &blbuf[0], length);

	actlen = LZ4_compress_HC(&blbuf[0], &zblbuf[length + sizeof(struct zdesc_t)], bllen, maxlen, 12);
	zbllen = length + sizeof(struct zdesc_t) + actlen;

	z = (struct zdesc_t *)&zblbuf[length];
	z->magic[0] = 'L';
	z->magic[1] = 'Z';
	z->magic[2] = '4';
	z->magic[3] = ' ';
	z->ssize[0] = (actlen >> 24) & 0xff;
	z->ssize[1] = (actlen >> 16) & 0xff;
	z->ssize[2] = (actlen >>  8) & 0xff;
	z->ssize[3] = (actlen >>  0) & 0xff;
	z->dsize[0] = (bllen >> 24) & 0xff;
	z->dsize[1] = (bllen >> 16) & 0xff;
	z->dsize[2] = (bllen >>  8) & 0xff;
	z->dsize[3] = (bllen >>  0) & 0xff;
	crc = crc32(crc, (const uint8_t *)(&z->ssize[0]), 4);
	crc = crc32(crc, (const uint8_t *)(&z->dsize[0]), 4);
	crc = crc32(crc, (const uint8_t *)(&zblbuf[length + sizeof(struct zdesc_t)]), actlen);
	z->crc[0] = (crc >> 24) & 0xff;
	z->crc[1] = (crc >> 16) & 0xff;
	z->crc[2] = (crc >> 8) & 0xff;
	z->crc[3] = (crc >> 0) & 0xff;

	zblfp = fopen(zblpath, "w+b");
	if(zblfp == NULL)
	{
		printf("Open zbootloader error\r\n");
		free(zblbuf);
		return -1;
	}
	if(fwrite(zblbuf, 1, zbllen, zblfp) != zbllen)
	{
		printf("Write bootloader error\r\n");
		free(blbuf);
		free(zblbuf);
		fclose(zblfp);
		return -1;
	}

	free(blbuf);
	free(zblbuf);
	fclose(zblfp);

	printf("Compressed %d bytes into %d bytes ==> %f%%\r\n", bllen, actlen, actlen * 100.0 / bllen);
	return 0;
}

