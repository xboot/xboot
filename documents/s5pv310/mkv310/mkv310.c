#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * IROM code reads first 14K bytes from boot device.
 * It then calculates the checksum of 14K-4 bytes and compare with data at
 * 14K-4 offset.
 */

#define CHECKSUM_OFFSET		(14*1024 - 4)
#define BUFSIZE			(16*1024)

int main (int argc, char *argv[])
{
	FILE * fp;	
	unsigned char buffer[BUFSIZE];
	unsigned char header[4];
	unsigned int checksum, count;
	int i, len;

	if(argc != 2)
	{
		printf("Usage: mkv310 <file>\n");
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
	count = (len < CHECKSUM_OFFSET) ? len : CHECKSUM_OFFSET;

	fseek(fp, 0L, SEEK_SET);
	memset(buffer, 0, sizeof(buffer));
	if(fread(buffer, 1, count, fp) != count)
	{
		printf("Can't read %s\n", argv[1]);
		fclose(fp);
		return -1;
	}

	for (i = 0, checksum = 0; i < CHECKSUM_OFFSET; i++)
	{
		checksum += buffer[i] & 0xff;
	}

	header[3] = (checksum >> 24) & 0xff;
	header[2] = (checksum >> 16) & 0xff;
	header[1] = (checksum >> 8) & 0xff;
	header[0] = (checksum >> 0) & 0xff;

	fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
	if(fwrite(header, 1, sizeof(header), fp) != sizeof(header))
	{
		printf("Write header error %s\n", argv[1]);
		fclose(fp);
		return -1;
	}

	fclose(fp);

	printf("The checksum is 0x%08x for %d bytes [0x%08x ~ 0x%08x]\n", checksum, count, 0, count - 1);
	return 0;
}
