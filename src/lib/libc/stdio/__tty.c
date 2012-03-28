/*
 * libc/stdio/__tty.c
 */

#include <stdio.h>

ssize_t __tty_read(FILE * f, unsigned char * buf, size_t size)
{
	return 0;
}

ssize_t __tty_write(FILE * f, const unsigned char * buf, size_t size)
{
	return 0;
}

fpos_t __tty_seek(FILE * f, fpos_t off, int whence)
{
	return 0;
}

int __tty_close(FILE * f)
{
	return 0;
}
