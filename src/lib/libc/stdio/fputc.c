/*
 * libc/stdio/fputc.c
 */

#include <stdio.h>

int fputc(int c, FILE * f)
{
	if (f == NULL)
	{
		errno = EBADF;
		return EOF;
	}
	if (f->error)
		return EOF;
	if (f->out.buf == NULL)
	{
		errno = EBADF;
		return EOF;
	}

	if (f->out.pos == f->out.limit)
	{
		if (f->out.limit == BUFSIZ)
		{
			if (fflush(f) == EOF)
				return EOF;
			if (__fill_stdio(f) == -1)
				return EOF;
		}
		else
		{
			f->out.limit++;
			if (f->in.buf != NULL)
				f->in.limit++;
		}
	}

	f->out.buf[f->out.pos++] = (unsigned char) (c & 0xff);
	f->out.dirty = 1;
	if (f->in.buf != NULL)
		f->in.buf[f->in.pos++] = (unsigned char) (c & 0xff);

	if (c == '\n')
	{
		if (fflush(f) == EOF)
			return EOF;
	}

	return c;
}

