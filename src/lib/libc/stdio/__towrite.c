/*
 * libc/stdio/__towrite.c
 */

#include <stdio.h>

int __towrite(FILE * f)
{
	f->mode |= f->mode-1;

	if(f->flags & (F_NOWR))
	{
		f->flags |= F_ERR;
		return EOF;
	}

	/* Clear read buffer (easier than summoning nasal demons) */
	f->rpos = f->rend = 0;

	/* Activate write through the buffer. */
	f->wpos = f->wbase = f->buf;
	f->wend = f->buf + f->buf_size;

	return 0;
}

/*
 * Link flush-on-exit code iff any stdio write functions are linked.
 */
void __fflush_on_exit()
{
	fflush(0);
}
