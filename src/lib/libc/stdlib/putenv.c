/*
 * libc/stdlib/putenv.c
 */

#include <string.h>
#include <malloc.h>
#include <stdlib.h>

extern int __put_env(char * str, size_t len, int overwrite);

int putenv(const char * str)
{
	char *s;
	const char *e, *z;

	if (!str)
	{
		errno = EINVAL;
		return -1;
	}

	e = NULL;
	for (z = str; *z; z++)
	{
		if (*z == '=')
			e = z;
	}

	if (!e)
	{
		errno = EINVAL;
		return -1;
	}

	s = strdup(str);
	if (!s)
		return -1;

	return __put_env(s, e - str, 1);
}
