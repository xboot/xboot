/*
 * libc/stdio/fputs.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

int fputs(const char * s, FILE * fp)
{
	size_t len;

	if(!fp)
		return -1;

	len = strlen(s);
	if(len == 0)
		return 0;

	if(write(fp->fd, (void *)s, len) < 0)
		return -1;

	return len;
}
