/*
 * libc/stdlib/getenv.c
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char * getenv(const char * name)
{
	char **p, *q;
	int len = strlen(name);

	if (!environ)
		return NULL;

	for (p = environ; (q = *p); p++)
	{
		if (!strncmp(name, q, len) && q[len] == '=')
		{
			return q + (len + 1);
		}
	}

	return NULL;
}
