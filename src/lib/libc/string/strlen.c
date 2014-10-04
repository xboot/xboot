/*
 * libc/string/strlen.c
 */

#include <types.h>
#include <string.h>

/*
 * Calculate the length of a string
 */
size_t strlen(const char * s)
{
	const char * sc;

	for (sc = s; *sc != '\0'; ++sc);
	return sc - s;
}
EXPORT_SYMBOL(strlen);
