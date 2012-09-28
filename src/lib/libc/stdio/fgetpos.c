/*
 * libc/stdio/fgetpos.c
 */

#include <xboot/module.h>
#include <stdio.h>

int fgetpos(FILE * f, fpos_t * pos)
{
	*pos = f->pos;
	return 0;
}
EXPORT_SYMBOL(fgetpos);
