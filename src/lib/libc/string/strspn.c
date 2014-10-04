/*
 * libc/string/strspn.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Finds in a string the first occurrence of a byte not in a set
 */
size_t strspn(const char * s, const char * accept)
{
	const char * p;
	const char * a;
	size_t count = 0;

	for (p = s; *p != '\0'; ++p)
	{
		for (a = accept; *a != '\0'; ++a)
		{
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}
	return count;
}
EXPORT_SYMBOL(strspn);
