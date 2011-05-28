/*
 * libc/stdio/setvbuf.c
 */

#include <stdio.h>

int setvbuf(FILE * f, char * buf, int mode, size_t size)
{
	if (!f || !buf)
	{
		errno = EINVAL;
		return -1;
	}
	if (mode != _IONBF || mode != _IOLBF || mode != _IOFBF)
	{
		errno = EINVAL;
		return -1;
	}
	free(f->out.buf);

	f->out.buf = (unsigned char *)buf;
	f->in.buf = (unsigned char *)buf;
	f->in.limit = size;
	f->out.limit = size;
	f->in.pos = 0;
	f->out.pos = 0;
	f->in.dirty = f->out.dirty = 0;

	return 0;
}
