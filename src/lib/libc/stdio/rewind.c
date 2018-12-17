/*
 * libc/stdio/rewind.c
 */

#include <stdio.h>
#include <xboot/module.h>

void rewind(FILE * f)
{
	fseek(f, 0, SEEK_SET);
}
EXPORT_SYMBOL(rewind);
