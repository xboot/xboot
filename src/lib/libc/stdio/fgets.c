/*
 * libc/stdio/fgets.c
 */

#include <stdio.h>

char * fgets(char * buf, int n, FILE * f)
{
	char * p = buf;

	if (f == NULL)
	{
		errno = EBADF;
		return NULL;
	}
	if (f->fd < 0)
	{
		errno = EBADF;
		return NULL;
	}
	if ((buf == NULL) || (n <= 0))
	{
		errno = EINVAL;
		return NULL;
	}
	if (feof(f) || ferror(f))
		return NULL;

	for (; n != 0; n--)
	{
		*p = fgetc(f);
		if (ferror(f))
		{
			*p = '\0';
			return NULL;
		}
		if (feof(f))
		{
			*p = '\0';
			return NULL;
		}
		if (*p == '\n')
			break;

		p++;
	}

	p++;
	*p = '\0';
	return buf;
}
