/*
 * libc/stdio/sscanf.c
 */

#include <stdio.h>

int sscanf(const char * buf, const char * fmt, ...)
{
	va_list ap;
	int rv;

	va_start(ap, fmt);
	rv = vsscanf(buf, fmt, ap);
	va_end(ap);

	return rv;
}
EXPORT_SYMBOL(sscanf);
