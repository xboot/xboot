/*
 * libc/stdio/fsetpos.c
 */

#include <stdio.h>

int fsetpos(FILE * f, const fpos_t * pos)
{
	return __fseeko(f, *(const off_t *)pos, SEEK_SET);
}
