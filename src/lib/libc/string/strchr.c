/*
 * libc/string/strchr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Finds the first occurrence of a byte in a string
 */
char * strchr(const char * s, int c)
{
	for (; *s != (char)c; ++s)
		if (*s == '\0')
			return NULL;
	return (char *)s;
}
EXPORT_SYMBOL(strchr);
