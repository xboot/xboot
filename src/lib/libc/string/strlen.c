/*
 * libc/string/strlen.c
 */

#include <types.h>
#include <string.h>

size_t strlen(const char * s)
{
	const char * sc;

	for (sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

