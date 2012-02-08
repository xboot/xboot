/*
 * libc/stdio/stdio.c
 */

#include <fs/fileio.h>
#include <stdio.h>

/*
 * Small standard IO/seek/close functions.
 * These maintain the 'known seek offset' for seek optimization.
 */
int __sread(void * cookie, char * buf, int n)
{
	FILE * fp = cookie;
	int ret;

	ret = read(fp->_file, buf, n);

	/*
	 * if the read succeeded, update the current offset
	 */
	if (ret >= 0)
		fp->_offset += ret;
	else
		fp->_flags &= ~__SOFF;

	return (ret);
}

int __swrite(void * cookie, const char * buf, int n)
{
	FILE * fp = cookie;
	int ret;

	if (fp->_flags & __SAPP)
		lseek(fp->_file, 0, VFS_SEEK_END);
	fp->_flags &= ~__SOFF;

	ret = write(fp->_file, (void *)buf, n);
	return (ret);
}

fpos_t __sseek(void * cookie, fpos_t offset, int whence)
{
	FILE * fp = cookie;
	fpos_t ret;

	ret = lseek(fp->_file, offset, whence);

	if (ret == -1)
		fp->_flags &= ~__SOFF;
	else
	{
		fp->_flags |= __SOFF;
		fp->_offset = ret;
	}

	return (ret);
}

int __sclose(void * cookie)
{
	return (close(((FILE *)cookie)->_file));
}
