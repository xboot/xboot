/*
 * libc/stdio/ungetc.c
 */

#include <xboot/module.h>
#include <stdio.h>

int ungetc(int c, FILE * f)
{
	unsigned char ch = c & 0xff;

	if(fifo_put(f->fifo_read, &ch, 1) != 1)
		return EOF;

	return ch;
}
EXPORT_SYMBOL(ungetc);
