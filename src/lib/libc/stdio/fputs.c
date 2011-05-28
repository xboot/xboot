/*
 * libc/stdio/fputs.c
 */

#include <stdio.h>

int fputs(const char * s, FILE * f)
{
	int rc;

	if (s == NULL)
	{
		errno = EINVAL;
		return EOF;
	}

	rc = 0;
	while (*s != '\0')
	{
		if (fputc(*s, f) == EOF)
			return EOF;
		s++;
		rc++;
	}

	return rc;
}
