/*
 * libc/stdio/clearerr.c
 */

#include <stdio.h>

void clearerr(FILE * f)
{
	f->error = 0;
	f->eof = 0;
}
EXPORT_SYMBOL(clearerr);
