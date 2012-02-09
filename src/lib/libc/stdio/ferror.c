/*
 * libc/stdio/ferror.c
 */

#include <stdio.h>

int ferror(FILE * f)
{
	return !!(f->flags & F_ERR);
}
