#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE		(8 * 1024)
#define HEADER_SIZE		(16)

int main (int argc, char *argv[])
{
	FILE * fp;
	unsigned char buffer[BUFFER_SIZE];
	unsigned char header[HEADER_SIZE];
	unsigned int checksum, count;
	int i, len;

	if(argc != 2)
	{
		printf("Usage: mk4412 <file>\n");
		return -1;
	}

	fp = fopen(argv[1], "r+b");
	if(fp == NULL)
	{
		printf("Can not open file '%s'\n", argv[1]);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	count = (len < BUFFER_SIZE) ? len : BUFFER_SIZE;

	fseek(fp, 0L, SEEK_SET);
	memset(buffer, 0, sizeof(buffer));
	if(fread(buffer, 1, count, fp) != count)
	{
		printf("Can't read %s\n", argv[1]);
		fclose(fp);
		return -1;
	}

	for(i = 16, checksum = 0; i < count; i++)
	{
		checksum += buffer[i] & 0xff;
	}

	memset(header, 0, sizeof(header));

	header[3] = (0x1f >> 24) & 0xff;
	header[2] = (0x1f >> 16) & 0xff;
	header[1] = (0x1f >> 8) & 0xff;
	header[0] = (0x1f >> 0) & 0xff;

	header[7] = (checksum >> 24) & 0xff;
	header[6] = (checksum >> 16) & 0xff;
	header[5] = (checksum >> 8) & 0xff;
	header[4] = (checksum >> 0) & 0xff;

	header[0] ^= 0xbc;
	header[1] ^= 0xca;
	header[2] ^= 0xba;
	header[3] ^= 0xcb;
	header[4] ^= 0xcb;
	header[5] ^= 0xce;
	header[6] ^= 0xcd;
	header[7] ^= 0xdf;
	header[8] ^= 0xb7;
	header[9] ^= 0xba;
	header[10] ^= 0xbe;
	header[11] ^= 0xbb;
	header[12] ^= 0xba;
	header[13] ^= 0xad;
	header[14] ^= 0xdf;
	header[15] ^= 0xdf;

	for (i = 1; i < HEADER_SIZE; i++)
	{
		header[i] ^= header[i-1];
	}

	fseek(fp, 0L, SEEK_SET);
	if(fwrite(header, 1, sizeof(header), fp) != sizeof(header))
	{
		printf("Write header error %s\n", argv[1]);
		fclose(fp);
		return -1;
	}

	fclose(fp);

	printf("The checksum is 0x%08x for %ld bytes [0x%08x ~ 0x%08x]\n", checksum, (count - sizeof(header)), sizeof(header), (count - 1));
	return 0;
}
