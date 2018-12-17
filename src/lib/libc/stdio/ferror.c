/*
 * libc/stdio/ferror.c
 */

#include <stdio.h>
#include <xboot/module.h>

int ferror(FILE * f)
{
	return f->error;
}
EXPORT_SYMBOL(ferror);
