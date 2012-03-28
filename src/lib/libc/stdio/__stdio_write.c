/*
 * libc/stdio/__stdio_write.c
 */

#include <malloc.h>
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
	size_t buffer_size;
	ssize_t i;
	ssize_t ret;
	ssize_t cnt = 0;

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
				if((ret = __stdio_write_flush(f)))
					return ret;

				ret = __unbuffered_write(f, buf, i);
				if(ret <= 0)
					return ret;

				buf += i;
				size -= i;
				cnt = i;
				break;
			}
		}

		break;
	}

	/*
	 * remaining data without end of line will be treated as block
	 */
	case _IOFBF:
		buffer_size = f->fifo_write->size;

		/*
		 * check if all data can be put in buffer
		 */
		if (fifo_len(f->fifo_write) + size > buffer_size)
		{
			/*
			 * write all data present in buffer
			 */
			if((ret = __stdio_write_flush(f)))
				return ret;

			/*
			 * write data directly to device if greater than buffer
			 */
			while(size > buffer_size)
			{
				ret = f->write(f, buf, size);

				if(ret < 0)
				{
					f->error = 1;
					return ret;
				}

				size -= ret;
				buf += ret;
				cnt += ret;
				f->pos += ret;
			}
		}

		/*
		 * fill buffer with remaining data
		 */
		fifo_put(f->fifo_write, (u8_t *)buf, size);
		f->pos += size;
		cnt += size;

		f->rwflush = &__stdio_write_flush;
		break;
	}

	return cnt;
}
