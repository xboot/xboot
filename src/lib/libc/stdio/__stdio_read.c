/*
 * libc/stdio/__stdio_read.c
 */

#include <stdio.h>

static ssize_t __unbuffered_read(FILE * f, unsigned char * buf, size_t size)
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
		return __unbuffered_read(f, buf, size);

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
