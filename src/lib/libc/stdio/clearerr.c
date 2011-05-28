/*
 * libc/stdio/clearerr.c
 */

#include <stdio.h>

void clearerr(FILE * f)
{
	if (f != NULL)
	{
		f->eof = 0;
		f->error = 0;
	}
}
