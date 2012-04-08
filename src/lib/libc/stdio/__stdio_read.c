/*
 * libc/stdio/__stdio_read.c
 */

#include <malloc.h>
#include <stdio.h>

static ssize_t __unbuffered_read(FILE * f, unsigned char * buf, size_t size)
{
	ssize_t cnt = 0;
    ssize_t bytes;

    while(size > 0)
    {
        bytes = f->read(f, buf, size);
        if(bytes <= 0)
        {
            if(bytes == 0)
                f->eof = 1;
            else
            	f->error = 1;
            break;
        }

        f->pos += bytes;
        size -= bytes;
        buf += bytes;
        cnt += bytes;
    }

    return cnt;
}

ssize_t __stdio_read(FILE * f, unsigned char * buf, size_t size)
{
	ssize_t local_size;
	size_t buffer_size;
	size_t s;
	unsigned char * local;
	ssize_t cnt = 0;
	ssize_t bytes;

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
		bytes = fifo_get(f->fifo_read, buf, size);
		size -= bytes;
		buf += bytes;
        cnt += bytes;
		f->pos += bytes;

		if(size > 0)
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
				bytes = f->read(f, buf, s);

				if(bytes <= 0)
				{
					if(bytes == 0)
						f->eof = 1;
					else
						f->error = 1;

					free(local);
					return cnt;
				}

				f->eof = 0;
				size -= bytes;
				buf += bytes;
				cnt += bytes;
				f->pos += bytes;
			}
		}

		/*
		 * read remaining data in local buffer
		 */
		for(local_size = 0; local_size < size; local_size += bytes)
		{
			bytes = f->read(f, local + local_size, buffer_size - local_size);

			if(bytes <= 0)
			{
				if(bytes == 0)
					f->eof = 1;
				else
					f->error = 1;
				break;
			}
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
