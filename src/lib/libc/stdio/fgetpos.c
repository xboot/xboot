/*
 * libc/stdio/fgetpos.c
 */

#include <stdio.h>

int fgetpos(FILE * f, fpos_t * pos)
{
	off_t off = __ftello(f);

	if (off < 0)
		return -1;

	*(off_t *)pos = off;
	return 0;
}
