/*
 * libc/stdio/ferror.c
 */

#include <xboot/module.h>
#include <stdio.h>

int ferror(FILE * f)
{
	return f->error;
}
EXPORT_SYMBOL(ferror);
