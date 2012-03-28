/*
 * libc/stdio/__stdio_read.c
 */

#include <malloc.h>
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
	ssize_t ret, local_size;
	size_t buffer_size;
	size_t s;
	unsigned char * local;
	ssize_t cnt = 0;

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
		buffer_size = f->fifo_read->size;

		local = malloc(buffer_size);
		if(!local)
			return ENOMEM;

		/*
		 * get data from buffer
		 */
		ret = fifo_get(f->fifo_read, buf, size);
		size -= ret;
		buf += ret;
        cnt += ret;
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
			while(size > buffer_size)
			{
				s = (size / buffer_size) * buffer_size;
				ret = f->read(f, buf, s);

				if(ret <= 0)
				{
					if(ret == 0)
						f->eof = 1;
					else
						f->error = 1;

					free(local);
					return ret;
				}

				f->eof = 0;
				size -= ret;
				buf += ret;
				cnt += ret;
				f->pos += ret;
			}
		}

		/*
		 * read remaining data in local buffer
		 */
		for(local_size = 0; local_size < size; local_size += ret)
		{
			ret = f->read(f, local + local_size, buffer_size - local_size);

			if(ret < 0)
			{
				f->error = 1;

				free(local);
				return ret;
			}

			if(ret == 0)
				break;
		}

		memcpy(buf, local, size);
		f->pos += size;
		cnt += size;

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
		}
		else
		{
			/*
			 * not enough data have been read
			 */
			f->eof = 1;
		}

		free(local);
		break;
	}

	default:
		break;
	}

	return cnt;
}
