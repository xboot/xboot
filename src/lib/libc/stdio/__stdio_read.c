/*
 * libc/stdio/__stdio_read.c
 */

#include <malloc.h>
#include <stdio.h>

static ssize_t __unbuffered_read(FILE * f, unsigned char * buf, size_t size)
{
	unsigned char * p = buf;
	ssize_t cnt = 0;
    ssize_t bytes;

    while(size > 0)
    {
        bytes = f->read(f, p, size);
        if(bytes <= 0)
        {
            if(bytes == 0)
                f->eof = 1;
            else
            	f->error = 1;
            break;
        }

        size -= bytes;
        cnt += bytes;
        p += bytes;
    }

    return cnt;
}

ssize_t __stdio_read(FILE * f, unsigned char * buf, size_t size)
{
	unsigned char * p = buf;
	ssize_t cnt = 0;
	ssize_t bytes;
	ssize_t div, rem, tmp;

    if(!f->read)
		return EINVAL;

	switch(f->mode)
	{
	case _IONBF:
	case _IOLBF:
		bytes = __unbuffered_read(f, p, size);
		f->pos += bytes;
		return bytes;

	case _IOFBF:
	{
		bytes = fifo_get(f->fifo_read, p, size);
		size -= bytes;
        cnt += bytes;
		f->pos += bytes;
		p += bytes;

		if(size > 0)
		{
			f->rwflush = &__stdio_no_flush;

			div = (size / f->bufsz);
			rem = (size % f->bufsz);

			if(div > 0)
			{
				bytes = __unbuffered_read(f, p, (div * f->bufsz));
				size -= bytes;
				cnt += bytes;
				f->pos += bytes;
				p += bytes;

				if(bytes <= 0)
					break;
			}

			if(rem > 0)
			{
				bytes = __unbuffered_read(f, f->buf, f->bufsz);

				if(bytes < rem)
					tmp = bytes;
				else
					tmp = rem;
				memcpy(p, f->buf, tmp);
				size -= tmp;
				cnt += tmp;
				f->pos += tmp;
				p += bytes;

				if(bytes >= tmp)
				{
					if(bytes > tmp)
					{
						fifo_put(f->fifo_read, f->buf + tmp, bytes - tmp);
						f->rwflush = &__stdio_read_flush;
						f->eof = 0;
					}
				}
				else
				{
					f->eof = 1;
				}
			}
		}
		break;
	}

	default:
		break;
	}

	return cnt;
}
