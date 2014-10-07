/*
 * libc/stdio/__stdio_flush.c
 */

#include <malloc.h>
#include <stdio.h>

int __stdio_no_flush(FILE * f)
{
	return 0;
}

int	__stdio_read_flush(FILE * f)
{
	f->seek(f, f->pos, SEEK_SET);
	fifo_clear(f->fifo_read);

	f->rwflush = &__stdio_no_flush;
	return 0;
}

int __stdio_write_flush(FILE * f)
{
	unsigned char * p;
	size_t size;
	ssize_t ret;

	if(!f)
		return -1;

	p = f->buf;
	size = fifo_get(f->fifo_write, p, f->bufsz);

	while(size > 0)
	{
		ret = f->write(f, p, size);

		if(ret <= 0)
		{
			f->error = 1;
			fifo_put(f->fifo_write, p, size);
			return -1;
		}

		size -= ret;
		p += ret;
	};


	f->rwflush = &__stdio_no_flush;
	return 0;
}
