/*
 * libc/stdio/fwrite.c
 */

#include <stdio.h>

size_t fwrite(const void * buf, size_t size, size_t count, FILE * f)
{
	const unsigned char * p = buf;
	size_t i;

	for(i = 0; i < count; i++)
	{
		if(__stdio_write(f, p, size) != size)
			break;

		p += size;
	}

	return i;
}
EXPORT_SYMBOL(fwrite);
