/*
 * libc/stdio/feof.c
 */

#include <stdio.h>
#include <xboot/module.h>

int feof(FILE * f)
{
	return f->eof;
}
EXPORT_SYMBOL(feof);
