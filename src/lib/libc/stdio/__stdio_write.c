/*
 * libc/stdio/__stdio_write.c
 */

#include <malloc.h>
#include <stdio.h>

static ssize_t __unbuffered_write(FILE * f, const unsigned char * buf, size_t size)
{
	unsigned char * p = (unsigned char *)buf;
	ssize_t cnt = 0;
	ssize_t bytes;

	while(size > 0)
	{
		bytes = f->write(f, p, size);

		if(bytes <= 0)
		{
			f->error = 1;
			break;
		}

		size -= bytes;
		cnt += bytes;
		p += bytes;
	}

	return cnt;
}

ssize_t __stdio_write(FILE * f, const unsigned char * buf, size_t size)
{
	unsigned char * p = (unsigned char *)buf;
	ssize_t cnt = 0;
	ssize_t bytes;
	size_t bufsz;
	int i, ret;

	if (!f->write)
		return EINVAL;

	switch (f->mode)
	{
	case _IONBF:
	{
		bytes = __unbuffered_write(f, p, size);
		f->pos += bytes;
		return bytes;
	}

	case _IOLBF:
	{
		for(i = size; i > 0; i--)
		{
			if(p[i - 1] == '\n')
			{
				ret = __stdio_write_flush(f);
				if(ret != 0)
					return cnt;

				bytes = __unbuffered_write(f, p, i);
				if(bytes <= 0)
					return cnt;

				size -= bytes;
				cnt = bytes;
				f->pos += bytes;
				p += bytes;

				break;
			}
		}

		if(size > 0)
		{
			bufsz = f->fifo_write->size;

			if(fifo_avail(f->fifo_write) + size > bufsz)
			{
				ret = __stdio_write_flush(f);
				if(ret != 0)
					return cnt;

				while(size > bufsz)
				{
					bytes = f->write(f, p, size);
					if(bytes < 0)
					{
						f->error = 1;
						return cnt;
					}

					size -= bytes;
					cnt += bytes;
					f->pos += bytes;
					p += bytes;
				}
			}

			bytes = fifo_put(f->fifo_write, (u8_t *)p, size);
			size -= bytes;
			cnt += bytes;
			f->pos += bytes;
			p += bytes;

			f->rwflush = &__stdio_write_flush;
		}
		break;
	}

	case _IOFBF:
	{
		bufsz = f->fifo_write->size;

		if(fifo_avail(f->fifo_write) + size > bufsz)
		{
			ret = __stdio_write_flush(f);
			if(ret != 0)
				return cnt;

			while(size > bufsz)
			{
				bytes = f->write(f, p, size);
				if(bytes < 0)
				{
					f->error = 1;
					return cnt;
				}

				size -= bytes;
				cnt += bytes;
				f->pos += bytes;
				p += bytes;
			}
		}

		bytes = fifo_put(f->fifo_write, (u8_t *)p, size);
		size -= bytes;
		cnt += bytes;
		f->pos += bytes;
		p += bytes;

		f->rwflush = &__stdio_write_flush;

		break;
	}
	}

	return cnt;
}
