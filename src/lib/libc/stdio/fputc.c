/*
 * libc/stdio/fputc.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

int fputc(int c, FILE * fp)
{
	char ch = c;

	if(!fp)
		return -1;

	if(write(fp->fd, (void *)&ch, 1) != 1)
		return -1;

	return c;
}
