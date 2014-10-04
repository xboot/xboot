/*
 * libc/environ/setenv.c
 */

#include <string.h>
#include <malloc.h>
#include <environ.h>

extern int __put_env(char * str, size_t len, int overwrite);

int setenv(const char * name, const char * val, int overwrite)
{
	const char *z;
	char *s;
	size_t l1, l2;

	if(!name || !name[0])
		return -1;

	l1 = 0;
	for(z = name; *z; z++)
	{
		l1++;
		if(*z == '=')
			return -1;
	}

	l2 = strlen(val);

	s = malloc(l1 + l2 + 2);
	if(!s)
		return -1;

	memcpy(s, name, l1);
	s[l1] = '=';
	memcpy(s + l1 + 1, val, l2 + 1);

	return __put_env(s, l1 + 1, overwrite);
}
EXPORT_SYMBOL(setenv);
