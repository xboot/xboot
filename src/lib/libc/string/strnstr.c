/*
 * libc/string/strnstr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Locate a substring in a strin
 */
char * strnstr(const char * s1, const char * s2, size_t n)
{
	size_t l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;

	while (n >= l2)
	{
		n--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}

	return NULL;
}
EXPORT_SYMBOL(strnstr);
