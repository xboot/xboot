/*
 * libc/stdio/getc.c
 */

#include <stdio.h>
#include <xboot/module.h>

int getc(FILE * f)
{
	return fgetc(f);
}
EXPORT_SYMBOL(getc);
