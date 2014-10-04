/*
 * libc/stdio/fsetpos.c
 */

#include <stdio.h>

int fsetpos(FILE * f, const fpos_t * pos)
{
	return fseek(f, *pos, SEEK_SET);
}
EXPORT_SYMBOL(fsetpos);
