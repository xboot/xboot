/*
 * libc/stdio/fread.c
 */

#include <stdio.h>

size_t fread(void * buf, size_t size, size_t count, FILE * f)
{
	size_t i, j;
	unsigned char * p;

	if ((buf == NULL) || (f == NULL))
	{
		errno = EINVAL;
		return 0;
	}
	if ((size == 0) || (count == 0))
		return 0;

	p = buf;
	for (i = 0; i < count; i++)
	{
		for (j = 0; j < size; j++)
		{
			*p++ = (unsigned char) fgetc(f);
			if (feof(f) || ferror(f))
				return i;
		}
	}

	return i;
}
