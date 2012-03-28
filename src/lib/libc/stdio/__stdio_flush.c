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
	/*
	 * seek fd to real pos and flush prefetched data in read buffer
	 */
	f->seek(f, f->pos, SEEK_SET);
	fifo_reset(f->fifo_read);

	/*
	 * read buffer is empty here
	 */
	f->rwflush = &__stdio_no_flush;

	return 0;
}

int __stdio_write_flush(FILE * f)
{
	unsigned char *p, *q;
	size_t size;
	ssize_t ret;

	p = q = malloc(f->fifo_write->size);
	if(!q)
		return ENOMEM;

	size = fifo_get(f->fifo_write, q, f->fifo_write->size);

	/*
	 * write remaining data present in buffer
	 */
	while(size > 0)
	{
		ret = f->write(f, p, size);

		if(ret <= 0)
		{
			f->error = 1;
			fifo_put(f->fifo_write, p, size);

			free(q);
			return -1;
		}

		size -= ret;
		p += ret;
	};

	/*
	 * write buffer is empty here
	 */
	f->rwflush = &__stdio_no_flush;

	free(q);
	return 0;
}
