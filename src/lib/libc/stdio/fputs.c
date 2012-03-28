/*
 * libc/stdio/fputs.c
 */

#include <stdio.h>

int fputs(const char * s, FILE * f)
{
	return (__stdio_write(f, (unsigned char *)s, strlen(s)) ? EOF : 0);
}

