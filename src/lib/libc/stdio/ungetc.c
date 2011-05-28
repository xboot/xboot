/*
 * libc/stdio/ungetc.c
 */

#include <stdio.h>

int ungetc(int c, FILE * f)
{
	if (f == NULL)
	{
		errno = EBADF;
		return EOF;
	}
	if (f->error)
		return EOF;
	if (f->in.buf == NULL)
	{
		errno = EBADF;
		return EOF;
	}
	if (c == EOF)
		return EOF;

	if (f->in.pos == 0)
	{
		errno = EOVERFLOW;
		return EOF;
	}

	f->eof = 0;

	f->in.pos--;
	f->in.buf[f->in.pos] = (unsigned char) (c & 0xff);
	if (f->out.buf != NULL)
	{
		f->out.pos--;
	}

	return c;
}
