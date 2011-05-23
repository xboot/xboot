/*
 * libc/stdio/fscanf.c
 */

#include <types.h>
#include <sizes.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

int fscanf(FILE * fp, const char * fmt, ...)
{
	va_list ap;
	char * buf;
	int rv;

	buf = malloc(SZ_4K);
	if(!buf)
		return 0;

	read(fp->fd, (void *)buf, SZ_4K);

	va_start(ap, fmt);
	rv = vsscanf(buf, fmt, ap);
	va_end(ap);

	free(buf);
	return rv;
}
