/*
 * libc/stdio/fflush.c
 */

#include <xboot/module.h>
#include <stdio.h>

int fflush(FILE * f)
{
	if(!f->write)
		return EINVAL;

	return __stdio_write_flush(f);
}
EXPORT_SYMBOL(fflush);
