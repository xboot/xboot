/*
 * libc/stdio/fprintf.c
 */

#include <xboot/module.h>
#include <malloc.h>
#include <stdio.h>

int fprintf(FILE * f, const char * fmt, ...)
{
	va_list ap;
	char * buf;
	int len;
	int rv;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	if(len < 0)
		return 0;
	buf = malloc(len + 1);
	if(!buf)
		return 0;
	rv = vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);

	rv = (fputs(buf, f) < 0) ? 0 : rv;
	free(buf);

	return rv;
}
EXPORT_SYMBOL(fprintf);
