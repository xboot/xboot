/*
 * libc/stdio/fscanf.c
 */

#include <stdio.h>

int fscanf(FILE * f, const char * fmt, ...)
{
	va_list ap;
	char * buf;
	int rv;

	buf = malloc(SZ_4K);
	if(!buf)
		return 0;

	//TODO
	read(f->fd, (void *)buf, SZ_4K);

	va_start(ap, fmt);
	rv = vsscanf(buf, fmt, ap);
	va_end(ap);

	free(buf);
	return rv;
}
