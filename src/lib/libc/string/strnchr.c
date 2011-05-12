/*
 * libc/string/strnchr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

char * strnchr(const char * s, size_t n, int c)
{
	for (; n-- && *s != '\0'; ++s)
		if (*s == (char)c)
			return (char *)s;
	return NULL;
}
