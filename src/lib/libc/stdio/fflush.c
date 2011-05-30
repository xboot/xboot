/*
 * libc/stdio/fflush.c
 */

#include <stdio.h>

int fflush(FILE * f)
{
	ssize_t wr;

	if (f == NULL)
		return __flush_stdio();

	if (f->error)
		return EOF;

	if (f->out.buf == NULL)
	{
		f->ofs += f->in.pos;
		return 0;
	}
	if (!f->out.dirty)
	{
		f->ofs += f->out.pos;
		return 0;
	}

	wr = write(f->fd, f->out.buf, f->out.pos);
	if (wr != f->out.pos)
	{
		f->error = 1;
		return EOF;
	}

	f->out.dirty = 0;
	f->ofs += f->out.pos;

	if (f->in.buf != NULL)
	{
		memmove(f->in.buf, f->in.buf + f->in.pos,
				f->in.limit - f->in.pos);
		f->in.limit -= f->in.pos;
		f->in.pos = 0;
	}
	memmove(f->out.buf, f->out.buf + f->out.pos,
			f->out.limit - f->out.pos);
	f->out.limit -= f->out.pos;
	f->out.pos = 0;

	return 0;
}
