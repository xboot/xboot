/*
 * libc/stdio/fgetc.c
 */

#include <stdio.h>

int fgetc(FILE * f)
{
	int c;

	if (f == NULL)
	{
		errno = EBADF;
		return EOF;
	}
	if (f->eof || f->error)
		return EOF;
	if (f->in.buf == NULL)
	{
		errno = EBADF;
		return EOF;
	}

	if (f->in.pos == f->in.limit)
	{
		if (fflush(f) != 0)
			return EOF;
		if (fill_stream(f) != 0)
			return EOF;
		if (f->eof)
			return EOF;
	}

	c = f->in.buf[f->in.pos++];
	if (f->out.buf != NULL)
		f->out.pos++;

	return c;
}
