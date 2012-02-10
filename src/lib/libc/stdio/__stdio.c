/*
 * libc/stdio/__stdio.c
 */

#include <fs/fileio.h>
#include <stdio.h>

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
