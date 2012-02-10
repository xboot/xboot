/*
 * libc/stdio/fputs.c
 */

#include <stdio.h>

int fputs(const char * s, FILE * f)
{
	size_t l = strlen(s);

	if(!l)
		return 0;

	return (int)fwrite(s, l, 1, f) - 1;
}

