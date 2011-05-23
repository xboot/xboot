/*
 * libc/stdio/fgetc.c
 */

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <stdio.h>

int fgetc(FILE * fp)
{
	char c;

	if(!fp)
		return -1;

	if(read(fp->fd, (void *)&c, 1) != 1)
		return -1;

	return c;
}
