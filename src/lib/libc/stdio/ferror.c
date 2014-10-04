/*
 * libc/stdio/ferror.c
 */

#include <stdio.h>

int ferror(FILE * f)
{
	return f->error;
}
EXPORT_SYMBOL(ferror);
