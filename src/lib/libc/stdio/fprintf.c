/*
 * libc/stdio/fprintf.c
 */

#include <stdarg.h>
#include <sizes.h>
#include <stdio.h>
#include <xboot/module.h>

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
