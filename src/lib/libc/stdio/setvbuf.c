/*
 * libc/stdio/setvbuf.c
 */

#include <stdio.h>
#include <xboot/module.h>

int setvbuf(FILE * f, char * buf, int mode, size_t size)
{
	f->rwflush(f);
	f->mode = mode;

	return 0;
}
EXPORT_SYMBOL(setvbuf);
