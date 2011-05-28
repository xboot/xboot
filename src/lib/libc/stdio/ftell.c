/*
 * libc/stdio/ftell.c
 */

#include <stdio.h>

loff_t ftell(FILE * f)
{
	if (f == NULL)
	{
		errno = EBADF;
		return -1;
	}

	return f->ofs;
}

