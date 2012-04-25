/*
 * libc/stdio/setvbuf.c
 */

#include <stdio.h>

int setvbuf(FILE * f, char * buf, int mode, size_t size)
{
	f->rwflush(f);
	f->mode = mode;

	return 0;
}
