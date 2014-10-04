/*
 * libc/string/strpbrk.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Finds in a string the first occurrence of a byte/wchar_t in a set
 */
char * strpbrk(const char * s1, const char * s2)
{
	const char * sc1, * sc2;

	for (sc1 = s1; *sc1 != '\0'; ++sc1)
	{
		for (sc2 = s2; *sc2 != '\0'; ++sc2)
		{
			if (*sc1 == *sc2)
				return (char *)sc1;
		}
	}

	return NULL;
}
EXPORT_SYMBOL(strpbrk);
