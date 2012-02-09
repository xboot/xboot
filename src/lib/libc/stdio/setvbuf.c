/*
 * libc/stdio/setvbuf.c
 */

#include <stdio.h>

int setvbuf(FILE * f, char * buf, int type, size_t size)
{
	f->lbf = EOF;

	if (type == _IONBF)
		f->buf_size = 0;
	else if (type == _IOLBF)
		f->lbf = '\n';

	f->flags |= F_SVB;

	return 0;
}
