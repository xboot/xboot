/*
 * libc/stdlib/putenv.c
 */

#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int __put_env(char * str, size_t len, int overwrite)
{
	static size_t __environ_size;
	static char ** __environ_alloc;
	static char * const null_environ = { NULL };
	char **p, *q;
	char **newenv;
	size_t n;

	if (!environ)
		environ = (char **) null_environ;

	n = 1;
	for (p = environ; (q = *p); p++)
	{
		n++;
		if (!strncmp(q, str, len))
		{
			if (!overwrite)
				free(str);
			else
				*p = str;
			return 0;
		}
	}

	if (__environ_alloc && environ != __environ_alloc)
	{
		free(__environ_alloc);
		__environ_alloc = NULL;
	}

	if (n < __environ_size)
	{
		p[1] = NULL;
		*p = str;
		return 0;
	}
	else
	{
		if (__environ_alloc)
		{
			newenv = realloc(__environ_alloc,
					(__environ_size << 1) * sizeof(char *));
			if (!newenv)
				return -1;

			__environ_size <<= 1;
		}
		else
		{
			size_t newsize = n + 32;
			newenv = malloc(newsize * sizeof(char *));
			if (!newenv)
				return -1;

			memcpy(newenv, environ, n * sizeof(char *));
			__environ_size = newsize;
		}
		newenv[n - 1] = str;
		newenv[n] = NULL;
		environ = newenv;
	}

	return 0;
}

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
