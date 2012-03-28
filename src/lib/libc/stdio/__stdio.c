/*
 * libc/stdio/__stdio.c
 */

#include <runtime.h>
#include <stdio.h>

int __stdio_init(FILE * f)
{
	f->rwflush = &__stdio_no_flush;

	f->fifo_read = fifo_alloc(BUFSIZ);
	f->fifo_write = fifo_alloc(BUFSIZ);

	f->pos = 0;
	f->mode = _IONBF;
	f->error = 0;
	f->eof = 0;

	return 0;
}
