/*
 * libc/stdio/putchar.c
 */

#include <stdio.h>
#include <xboot/module.h>

int putchar(int c)
{
	return fputc(c, stdout);
}
EXPORT_SYMBOL(putchar);
