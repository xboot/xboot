/*
 * libc/stdio/fread.c
 */

#include <stdio.h>
#include <xboot/module.h>

size_t fread(void * buf, size_t size, size_t count, FILE * f)
{
	size_t l = size * count;
	size_t r = __stdio_read(f, (unsigned char *)buf, l);
	return r / size;
}
EXPORT_SYMBOL(fread);
