#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLKSIZE				(512)

#define LOADER_POSITION		(64)
#define BOOTLOADER_POSITION	(92)

int main(int argc, char *argv[])
{
	FILE * fp;
	char * buffer;
	int length, reallen;
	int nbytes, filelen;

	if(argc != 4)
	{
		printf("Usage: mk3288 <destination> <loader> <bootloader>\n");
		return -1;
	}

	length = 32 * 1024 * 1024;
	buffer = malloc(length);
	memset(buffer, 0, length);

	/* loader */
	fp = fopen(argv[2], "r+b");
	if(fp == NULL)
	{
		printf("Open file loader error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[(LOADER_POSITION - 1) * BLKSIZE], 1, filelen, fp);
	if(nbytes != filelen)
	{
		printf("Read file loader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* bootloader */
	fp = fopen(argv[3], "r+b");
	if(fp == NULL)
	{
		printf("Open file bootloader error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	reallen = (BOOTLOADER_POSITION - 1) * BLKSIZE + filelen;
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[(BOOTLOADER_POSITION - 1) * BLKSIZE], 1, filelen, fp);
	if(nbytes != filelen)
	{
		printf("Read file bootloader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* destination */
	fp = fopen(argv[1], "w+b");
	if(fp == NULL)
	{
		printf("Destination file open error\n");
		free(buffer);
		return -1;
	}

	nbytes = fwrite(buffer, 1, reallen, fp);
	if(nbytes != reallen)
	{
		printf("Destination file write error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}

	free(buffer);
	fclose(fp);

	printf("Generate destination file: %s\n", argv[1]);
	return 0;
}

