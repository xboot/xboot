/*
 * libc/stdio/fwrite.c
 */

#include <stdio.h>
#include <xboot/module.h>

size_t fwrite(const void * buf, size_t size, size_t count, FILE * f)
{
	size_t l = size * count;
	size_t r = __stdio_write(f, (const unsigned char *)buf, l);
	return r / size;
}
EXPORT_SYMBOL(fwrite);
