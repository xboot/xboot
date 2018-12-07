/*
 * libc/stdio/asprintf.c
 */

#include <stdio.h>

int asprintf(char ** s, const char * fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = vasprintf(s, fmt, ap);
	va_end(ap);

	return ret;
}
EXPORT_SYMBOL(asprintf);
