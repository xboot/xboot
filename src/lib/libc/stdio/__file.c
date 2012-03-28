/*
 * libc/stdio/__file.c
 */

#include <fs/fileio.h>
#include <stdio.h>

ssize_t __file_read(FILE * f, unsigned char * buf, size_t size)
{
	return read(f->fd, (void *)buf, size);
}

ssize_t __file_write(FILE * f, const unsigned char * buf, size_t size)
{
	return write(f->fd, (void *)buf, size);
}

fpos_t __file_seek(FILE * f, fpos_t off, int whence)
{
	return lseek(f->fd, off, whence);
}

int __file_close(FILE * f)
{
	return close(f->fd);
}
