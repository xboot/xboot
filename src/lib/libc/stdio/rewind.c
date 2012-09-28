/*
 * libc/stdio/rewind.c
 */

#include <xboot/module.h>
#include <stdio.h>

void rewind(FILE * f)
{
	fseek(f, 0, SEEK_SET);
}
EXPORT_SYMBOL(rewind);
