/*
 * libc/stdio/fread.c
 */

#include <stdio.h>

size_t fread(void * buf, size_t size, size_t count, FILE * f)
{
	unsigned char * p = buf;
	size_t i;

	for(i = 0; i < count; i++)
	{
		if(__stdio_read(f, p, size) != size)
			break;

		p += size;
	}

	return i;
}
EXPORT_SYMBOL(fread);
