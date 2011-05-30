/*
 * libc/stdio/fclose.c
 */

#include <stdio.h>

int fclose(FILE * f)
{
	if (f == NULL)
	{
		errno = EINVAL;
		return EOF;
	}

	if (fflush(f) < 0)
		return EOF;

	if (close(f->fd) < 0)
		return EOF;

	__destroy_stdio(f);
	return 0;
}
