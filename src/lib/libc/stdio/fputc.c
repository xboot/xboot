/*
 * libc/stdio/fputc.c
 */

#include <stdio.h>

int fputc(int c, FILE * f)
{
	unsigned char ch = c & 0xff;

	if(__stdio_write(f, &ch, 1) <= 0)
		return EOF;

	return (ch);
}
EXPORT_SYMBOL(fputc);
