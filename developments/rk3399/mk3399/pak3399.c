#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	FILE * fp;
	char * buffer;
	int buflen = 64 * 1024 * 1024;
	int len, n;

	if(argc != 4)
	{
		printf("Usage: pak3399 <output file> <input file> <template file>\n");
		return -1;
	}

	buffer = malloc(buflen);
	memset(buffer, 0, buflen);

	/* template */
	fp = fopen(argv[3], "r+b");
	if(!fp)
	{
		printf("Open file '%s' error\n", argv[3]);
		free(buffer);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	n = fread(&buffer[0], 1, len, fp);
	if(n != len)
	{
		printf("Read template file error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* input */
	fp = fopen(argv[2], "r+b");
	if(!fp)
	{
		printf("Open file '%s' error\n", argv[2]);
		free(buffer);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	n = fread(&buffer[8 * 1024 * 1024], 1, len, fp);
	if(n != len)
	{
		printf("Read input file error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* output */
	fp = fopen(argv[1], "w+b");
	if(!fp)
	{
		printf("Open file '%s' error\n", argv[1]);
		free(buffer);
		return -1;
	}
	if(fwrite(&buffer[512], 16 * 1024 * 1024 - 512, 1, fp) != 1)
	{
		printf("Write output file error\n");
		free(buffer);
		close(fp);
		return -1;
	}
	free(buffer);
	close(fp);

	printf("Generate pakfile '%s' successed\n", argv[1]);
	return 0;
}
