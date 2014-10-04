/*
 * libc/stdio/sprintf.c
 */

#include <stdio.h>

int sprintf(char * buf, const char * fmt, ...)
{
	va_list ap;
	int rv;

	va_start(ap, fmt);
	rv = vsnprintf(buf, ~(size_t)0, fmt, ap);
	va_end(ap);

	return rv;
}
EXPORT_SYMBOL(sprintf);
