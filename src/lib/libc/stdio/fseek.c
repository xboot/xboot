/*
 * libc/stdio/fseek.c
 */

#include <stdio.h>

int fseek(FILE * f, loff_t offset, int whence)
{
	loff_t ofs;

	if (f == NULL)
	{
		errno = EBADF;
		return -1;
	}

	if (fflush(f) == EOF)
		return -1;

	ofs = lseek(f->fd, offset, whence);
	if (ofs < 0)
		return -1;
	f->ofs = ofs;

	return __fill_stdio(f);
}
