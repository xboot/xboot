/*
 * libc/stdio/fputs.c
 */

#include <string.h>
#include <stdio.h>
#include <xboot/module.h>

int fputs(const char * s, FILE * f)
{
	return ((__stdio_write(f, (unsigned char *)s, strlen(s)) <= 0) ? EOF : 0);
}
EXPORT_SYMBOL(fputs);
