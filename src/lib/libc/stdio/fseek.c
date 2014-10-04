/*
 * libc/stdio/fseek.c
 */

#include <stdio.h>

int fseek(FILE * f, fpos_t off, int whence)
{
	if(!f->seek)
		return EOF;

	f->rwflush(f);

	f->pos = f->seek(f, off, whence);
	if(f->pos >= 0)
		return 0;
	else
		return EOF;
}
EXPORT_SYMBOL(fseek);
