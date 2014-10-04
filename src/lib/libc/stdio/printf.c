/*
 * libc/stdio/printf.c
 */

#include <malloc.h>
#include <stdio.h>

int printf(const char * fmt, ...)
{
	va_list ap;
	char buf[SZ_4K];
	int rv;

	va_start(ap, fmt);
	rv = vsnprintf(buf, SZ_4K, fmt, ap);
	va_end(ap);

	rv = (fputs(buf, stdout) < 0) ? 0 : rv;
	fflush(stdout);
	return rv;
}
EXPORT_SYMBOL(printf);
