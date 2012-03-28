/*
 * libc/stdio/__stdio.c
 */

#include <fs/fileio.h>
#include <runtime.h>
#include <stdio.h>

int __stdio_no_flush(FILE * f)
{
	return 0;
}

int	__stdio_read_flush(FILE * f)
{
  return 0;
}

int __stdio_write_flush(FILE * f)
{
	return 0;
}

static int unbuffered_read(size_t size, FILE * f, unsigned char * ptr)
{
    ssize_t res;

    while (size)
    {
        res = f->read(f->handle, ptr, size);

        if (res <= 0)
        {
            if (res == 0)
                f->eof = 1;
            else
                f->error = 1;

            return res;
        }

        f->pos += res;
        size -= res;
        ptr += res;
    }

    return 1;
}

static int unbuffered_write(size_t size, FILE * f, const unsigned char * ptr)
{
	return 0;
}

int __stdio_read(FILE * f, unsigned char * buf, size_t len)
{
    return 0;
}

int __stdio_write(FILE * f, const unsigned char * buf, size_t len)
{
	return 0;
}

#if 0
size_t __stdio_read(FILE * f, unsigned char * buf, size_t len)
{
	struct iovec iov[2] = {
		{
			.iov_base = buf,
			.iov_len = len - !!f->buf_size
		}, {
			.iov_base = f->buf,
			.iov_len = f->buf_size
		}
	};
	ssize_t cnt;

	cnt = readv(f->fd, iov, 2);
	if(cnt <= 0)
	{
		f->flags |= F_EOF ^ ((F_ERR ^ F_EOF) & cnt);
		f->rpos = f->rend = 0;
		return cnt;
	}

	if (cnt <= iov[0].iov_len)
		return cnt;

	cnt -= iov[0].iov_len;
	f->rpos = f->buf;
	f->rend = f->buf + cnt;

	if (f->buf_size)
		buf[len - 1] = *f->rpos++;

	return len;
}

size_t __stdio_write(FILE * f, const unsigned char * buf, size_t len)
{
	struct iovec iovs[2] = {
		{
			.iov_base = f->wbase,
			.iov_len = f->wpos - f->wbase
		}, {
			.iov_base = (void *)buf,
			.iov_len = len
		}
	};
	struct iovec * iov = iovs;
	int iovcnt = 2;
	size_t rem = iov[0].iov_len + iov[1].iov_len;
	ssize_t cnt;

	f->wpos = f->wbase;
	for(;;)
	{
		cnt = writev(f->fd, iov, iovcnt);

		if(cnt == rem)
			return len;

		if(cnt < 0)
		{
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;

			return iovcnt == 2 ? 0 : len - iov[0].iov_len;
		}

		rem -= cnt;
		if(cnt > iov[0].iov_len)
		{
			cnt -= iov[0].iov_len;
			iov++;
			iovcnt--;
		}

		iov[0].iov_base = (char *)iov[0].iov_base + cnt;
		iov[0].iov_len -= cnt;
	}

	return 0;
}

off_t __stdio_seek(FILE * f, off_t off, int whence)
{
	off_t ret;

	ret = lseek(f->fd, off, whence);
	return ret;
}

int __stdio_close(FILE * f)
{
	return close(f->fd);
}

int __stdio_init(struct runtime_t * r)
{
	FILE * f;

	if(!r)
		return -1;

	/*
	 * stdin
	 */
	f = malloc(sizeof(FILE) + UNGET + BUFSIZ);
	if(!f)
		return -1;
	memset(f, 0, sizeof(FILE) + UNGET + BUFSIZ);

	f->fd = 0;
	f->buf = (unsigned char *)f + sizeof(FILE) + UNGET;
	f->buf_size = BUFSIZ;
	f->flags = F_PERM | F_NOWR;
	f->lock = -1;
	f->read = __stdio_read;
	f->write = NULL;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	r->__stdin = f;

	/*
	 * stdout
	 */
	f = malloc(sizeof(FILE) + UNGET + BUFSIZ);
	if(!f)
		return -1;
	memset(f, 0, sizeof(FILE) + UNGET + BUFSIZ);

	f->fd = 1;
	f->buf = (unsigned char *)f + sizeof(FILE) + UNGET;
	f->buf_size = BUFSIZ;
	f->flags = F_PERM | F_NORD;
	f->lbf = '\n';
	f->lock = -1;
	f->read = NULL;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	r->__stdout = f;

	/*
	 * stderr
	 */
	f = malloc(sizeof(FILE) + UNGET);
	if(!f)
		return -1;
	memset(f, 0, sizeof(FILE) + UNGET);

	f->fd = 2;
	f->buf = (unsigned char *)f + sizeof(FILE) + UNGET;
	f->buf_size = 0;
	f->flags = F_PERM | F_NORD;
	f->lbf = -1;
	f->lock = -1;
	f->read = NULL;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	r->__stderr = f;

	/*
	 * Open file list head
	 */
	r->ofl_head = NULL;

	return 0;
}

int __stdio_exit(struct runtime_t * r)
{
	if(!r)
		return -1;

	return 0;
}
#endif
