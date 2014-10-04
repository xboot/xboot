/*
 * libc/stdio/fprintf.c
 */

#include <malloc.h>
#include <stdio.h>

int fprintf(FILE * f, const char * fmt, ...)
{
	va_list ap;
	char buf[SZ_4K];
	int rv;

	va_start(ap, fmt);
	rv = vsnprintf(buf, SZ_4K, fmt, ap);
	va_end(ap);

	rv = (fputs(buf, f) < 0) ? 0 : rv;
	return rv;
}
EXPORT_SYMBOL(fprintf);
