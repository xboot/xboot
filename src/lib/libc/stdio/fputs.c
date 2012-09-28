/*
 * libc/stdio/fputs.c
 */

#include <xboot/module.h>
#include <stdio.h>

int fputs(const char * s, FILE * f)
{
	return ((__stdio_write(f, (unsigned char *)s, strlen(s)) <= 0) ? EOF : 0);
}
EXPORT_SYMBOL(fputs);
