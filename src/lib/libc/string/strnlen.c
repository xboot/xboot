/*
 * libc/string/strnlen.c
 */

#include <types.h>
#include <string.h>

/*
 * Determine the length of a fixed-size string
 */
size_t strnlen(const char * s, size_t n)
{
	const char * sc;

	for (sc = s; n-- && *sc != '\0'; ++sc);
	return sc - s;
}
EXPORT_SYMBOL(strnlen);
