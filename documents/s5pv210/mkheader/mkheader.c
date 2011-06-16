#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	FILE * fp;
	char header[16];
	char * buf;
	unsigned int size, checksum;
	int i;

	if(argc != 2)
	{
		printf("Usage: mkheader <file>\n");
		return -1;
	}

	fp = fopen(argv[1], "r+b");
	if(fp == NULL)
	{
		printf("Can not open file '%s'\n", argv[1]);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	if(ftell(fp) <= 16)
	{
		printf("The file '%s' is too small\n", argv[1]);
		fclose(fp);
		return -1;
	}

	fseek(fp, 0L, SEEK_SET);
	memset(header, 0, sizeof(header));
	if(fread(header, 1, sizeof(header), fp) != sizeof(header))
	{
		printf("Read file header error\n", argv[1]);
		fclose(fp);
		return -1;
	}

	size = (header[3]<<24) | (header[2]<<16) | (header[1]<<8) | (header[0]<<0);
	if(size <= 0 || size > 0x4000)
	{
		printf("Not a valid image, header error\n", argv[1]);
		fclose(fp);
		return -1;
	}

	buf = (char *)malloc(size);
	if(buf == NULL)
	{
		printf("Can not malloc buffer\n", argv[1]);
		fclose(fp);
		return -1;
	}

	memset(buf, 0x00, size);
	fseek(fp, 0L, SEEK_SET);
	if(fread(buf, 1, size, fp) != size)
	{
		printf("Read body file error\n", argv[1]);
		free(buf);
		fclose(fp);
		return -1;
	}

	for(i=16, checksum=0; i<size; i++)
	{
		checksum += buf[i] & 0xff;
	}

	header[11] = (checksum >> 24) & 0xff;
	header[10] = (checksum >> 16) & 0xff;
	header[9] = (checksum >> 8) & 0xff;
	header[8] = (checksum >> 0) & 0xff;

	fseek(fp, 0L, SEEK_SET);
	if(fwrite(header, 1, sizeof(header), fp) != sizeof(header))
	{
		printf("Write file header error\n", argv[1]);
		free(buf);
		fclose(fp);
		return -1;
	}

	free(buf);
	fclose(fp);

	printf("the checksum is 0x%08x for %d bytes [0x%08x ~ 0x%08x]\n", checksum, (size - sizeof(header)), sizeof(header), size);
	return 0;
}
