/*
 * libc/stdio/__stdio_write.c
 */

#include <stdio.h>

static ssize_t __unbuffered_write(FILE * f, const unsigned char * buf, size_t size)
{
	ssize_t cnt = 0;
	ssize_t ret;

	while(size > 0)
	{
		ret = f->write(f, buf, size);

		if(ret <= 0)
		{
			f->error = 1;
			return ret;
		}

		f->pos += ret;
		size -= ret;
		buf += ret;
		cnt += ret;
	}

	return cnt;
}

ssize_t __stdio_write(FILE * f, const unsigned char * buf, size_t size)
{
	ssize_t i;
	ssize_t ret;

	if (!f->write)
		return EINVAL;

	switch (f->mode)
	{
	case _IONBF:
		return __unbuffered_write(f, buf, size);

	case _IOLBF:
	{
		/*
		 * write all ended lines if any
		 */
		for(i = size; i > 0; i--)
		{
			if(buf[i - 1] == '\n')
			{
				if(__stdio_write_flush(f) != 0)
					return -1;

				ret = __unbuffered_write(f, buf, i);
				if(ret <= 0)
					return ret;

				buf += i;
				size -= i;
				break;
			}
		}

		break;
	}

	/*
	 * remaining data without end of line will be treated as block
	 */
	case _IOFBF:
#if 0
		/* check if all data can be put in buffer */
		if (stream_fifo_count(&f->fifo_write) + size
				> CONFIG_LIBC_STREAM_BUFFER_SIZE) {
			ssize_t ret;

			/* write all data present in buffer */
			if ((ret = __stdio_write_flush(f)))
				return ret;

			/* write data directly to device if greater than buffer */
			while (size > CONFIG_LIBC_STREAM_BUFFER_SIZE) {
				ret = f->ops->write(f->hndl, buf, size);

				if (ret < 0) {
					f->error = 1;
					return ret;
				}

				size -= ret;
				buf += ret;
				f->pos += ret;
			}
		}

		/* fill buffer with remaining data */
		void *tmp = (void*) buf;
		stream_fifo_pushback_array(&f->fifo_write,
				(stream_fifo_item_t*) tmp, size);
		f->pos += size;
		f->rwflush = &__stdio_write_flush;
#endif
		break;
	}

	return 0;
}
