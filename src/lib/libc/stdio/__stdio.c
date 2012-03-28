/*
 * libc/stdio/__stdio.c
 */

#include <runtime.h>
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

static ssize_t unbuffered_read(FILE * f, unsigned char * buf, size_t size)
{
	ssize_t cnt = 0;
    ssize_t ret;

    while(size > 0)
    {
        ret = f->read(f, buf, size);

        if(ret <= 0)
        {
            if(ret == 0)
                f->eof = 1;
            else
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

static ssize_t unbuffered_write(FILE * f, const unsigned char * buf, size_t size)
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

ssize_t __stdio_read(FILE * f, unsigned char * buf, size_t size)
{
    if(!f->read)
		return EINVAL;

	switch(f->mode)
	{
	case _IONBF:
	case _IOLBF:
		/*
		 * line buffered for LBF is non-sense so let's assume it's unbuffered
		 */
		return unbuffered_read(f, buf, size);

	case _IOFBF:
	{
#if 0
		unsigned char local[CONFIG_LIBC_STREAM_BUFFER_SIZE];
		ssize_t ret, local_size;

		/*
		 * get data from buffer
		 */
		ret = fifo_get(f->fifo_read, buf, size);
		size -= ret;
		buf += ret;
		f->pos += ret;

		if(size)
		{
			/*
			 * read buffer is empty here
			 */
			f->rwflush = &__stdio_no_flush;

			/*
			 * read more data directly from fd
			 */
			while(size > CONFIG_LIBC_STREAM_BUFFER_SIZE)
			{
				size_t s = (size / CONFIG_LIBC_STREAM_BUFFER_SIZE)
						* CONFIG_LIBC_STREAM_BUFFER_SIZE;
				ret = f->read(f, buf, s);

				if(ret <= 0)
				{
					if(ret == 0)
						f->eof = 1;
					else
						f->error = 1;

					return ret;
				}

				f->eof = 0;
				size -= ret;
				buf += ret;
				f->pos += ret;
			}
		}

		/*
		 * read remaining data in local buffer
		 */
		for(local_size = 0; local_size < size; local_size += ret)
		{
			ret = f->read(f, local + local_size,
					CONFIG_LIBC_STREAM_BUFFER_SIZE - local_size);

			if(ret < 0)
			{
				f->error = 1;
				return ret;
			}
			if(ret == 0)
				break;
		}

		memcpy(buf, local, size);
		f->pos += size;

		if(local_size >= size)
		{
			if(local_size > size)
			{
				/*
				 * if more data than needed, put in read buffer
				 */
				fifo_put(f->fifo_read, local + size, local_size - size);
				f->rwflush = &__stdio_read_flush;
			}

			return 1;
		}
		else
		{
			/*
			 * not enough data have been read
			 */
			f->eof = 1;
		}
#endif
	break;
	}

	default:
		break;
	}

	return 0;
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
		return unbuffered_write(f, buf, size);

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

				ret = unbuffered_write(f, buf, i);
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
