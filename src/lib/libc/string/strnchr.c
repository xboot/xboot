/*
 * libc/string/strnchr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Finds the first occurrence of a byte in a string
 */
char * strnchr(const char * s, size_t n, int c)
{
	for (; n-- && *s != '\0'; ++s)
		if (*s == (char)c)
			return (char *)s;
	return NULL;
}
EXPORT_SYMBOL(strnchr);
