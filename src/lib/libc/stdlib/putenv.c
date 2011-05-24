/*
 * libc/stdlib/putenv.c
 */

#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int putenv(const char * s)
{
	size_t len;
	int envc;
	int remove = 0;
	char *tmp;
	const char **ep;
	char **newenv;
	static char **origenv;

	if (!origenv)
		origenv = environ;

	if (!(tmp = strchr(s, '=')))
	{
		len = strlen(s);
		remove = 1;
	}
	else
		len = tmp - s;

	for (envc = 0, ep = (const char**) environ; (ep && *ep); ++ep)
	{
		if (*s == **ep && !memcmp(s, *ep, len) && (*ep)[len] == '=')
		{
			if (remove)
			{
				for (; ep[1]; ++ep)
					ep[0] = ep[1];
				ep[0] = 0;
				return 0;
			}
			*ep = s;
			return 0;
		}
		++envc;
	}

	if (tmp)
	{
		newenv = (char**) realloc(environ == origenv ? 0 : environ,
				(envc + 2) * sizeof(char*));
		if (!newenv)
			return -1;
		if (envc && (environ == origenv))
		{
			memcpy(newenv, origenv, envc * sizeof(char*));
		}
		newenv[envc] = (char*) s;
		newenv[envc + 1] = 0;
		environ = newenv;
	}

	return 0;
}
