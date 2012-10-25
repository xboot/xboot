/*
 * libc/stdio/getc.c
 */

#include <xboot/module.h>
#include <stdio.h>

int getc(FILE * f)
{
	return fgetc(f);
}
EXPORT_SYMBOL(getc);
