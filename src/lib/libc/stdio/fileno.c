/*
 * libc/stdio/fileno.c
 */

#include <types.h>
#include <stdarg.h>
#include <stdio.h>

int fileno(FILE * fp)
{
	return fp->fd;
}
