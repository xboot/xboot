/*
 * libc/stdio/feof.c
 */

#include <stdio.h>

int feof(FILE * f)
{
	if (f != NULL)
		return f->eof;

	return 0;
}
