/*
 * libc/stdio/fprintf.c
 */

#include <stdio.h>

int fprintf(FILE * fp, const char * fmt, ...)
{
	va_list ap;
	char * buf;
	int rv;

	buf = malloc(SZ_4K);
	if(!buf)
		return 0;

	va_start(ap, fmt);
	rv = vsnprintf(buf, ~(size_t)0, fmt, ap);
	va_end(ap);

	write(fp->fd, (void *)buf, rv);

	free(buf);
	return rv;
}
