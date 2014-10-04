/*
 * libc/string/strsep.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Extract token from string
 */
char * strsep(char ** s, const char * ct)
{
	char * sbegin = *s;
	char * end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;

	return sbegin;
}
EXPORT_SYMBOL(strsep);
