/*
 * libc/stdio/putchar.c
 */

#include <stdio.h>

int putchar(int c)
{
	return fputc(c, stdout);
}
EXPORT_SYMBOL(putchar);
