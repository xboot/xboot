/*
 * libc/stdio/feof.c
 */

#include <xboot/module.h>
#include <stdio.h>

int feof(FILE * f)
{
	return f->eof;
}
EXPORT_SYMBOL(feof);
