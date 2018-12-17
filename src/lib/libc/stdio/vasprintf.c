/*
 * libc/stdio/vasprintf.c
 */

#include <malloc.h>
#include <stdio.h>
#include <xboot/module.h>

int vasprintf(char ** s, const char * fmt, va_list ap)
{
	va_list ap2;
	int l;

	va_copy(ap2, ap);
	l = vsnprintf(0, 0, fmt, ap2);
	va_end(ap2);

	if((l < 0) || !(*s = malloc(l + 1)))
		return -1;
	return vsnprintf(*s, l + 1, fmt, ap);
}
EXPORT_SYMBOL(vasprintf);
