/*
 * libc/stdio/fileno.c
 */

#include <stdio.h>

int fileno(FILE * f)
{
	if (f == NULL)
	{
		errno = EBADF;
		return -1;
	}

	return f->fd;
}
