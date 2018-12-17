/*
 * libc/stdio/fflush.c
 */

#include <errno.h>
#include <stdio.h>
#include <xboot/module.h>

int fflush(FILE * f)
{
	if(!f->write)
		return EINVAL;

	return __stdio_write_flush(f);
}
EXPORT_SYMBOL(fflush);
