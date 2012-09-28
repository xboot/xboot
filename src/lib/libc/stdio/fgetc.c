/*
 * libc/stdio/fgetc.c
 */

#include <xboot/module.h>
#include <stdio.h>

int fgetc(FILE * f)
{
	unsigned char res;

	return ((__stdio_read(f, &res, 1) <= 0) ? EOF : res);
}
EXPORT_SYMBOL(fgetc);
