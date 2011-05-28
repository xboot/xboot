/*
 * libc/stdio/ferror.c
 */

#include <stdio.h>

int ferror(FILE * f)
{
	if (f != NULL)
		return f->error;
	return 0;
}
