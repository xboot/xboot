/*
 * libc/stdio/__stdio.c
 */

#include <runtime.h>
#include <stdio.h>

FILE * __file_alloc(int fd)
{
	FILE * f;

	f = malloc(sizeof(FILE));
	if(!f)
		return 0;

	if(fd == 0)
	{
		f->fd = fd;

		f->read = __tty_read;
		f->write = __tty_write;
		f->seek = __tty_seek;
		f->close = __tty_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 1)
	{
		f->fd = fd;

		f->read = __tty_read;
		f->write = __tty_write;
		f->seek = __tty_seek;
		f->close = __tty_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 2)
	{
		f->fd = fd;

		f->read = __tty_read;
		f->write = __tty_write;
		f->seek = __tty_seek;
		f->close = __tty_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IONBF;
		f->error = 0;
		f->eof = 0;
	}
	else
	{
		f->fd = fd;

		f->read = __file_read;
		f->write = __file_write;
		f->seek = __file_seek;
		f->close = __file_close;

		f->fifo_read = fifo_alloc(BUFSIZ);
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOFBF;
		f->error = 0;
		f->eof = 0;
	}

	return f;
}
