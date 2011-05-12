/*
 * libc/string/memcpy.c
 */

#include <types.h>
#include <string.h>

static void * __memcpy(void * dest, const void * src, size_t len)
{
	char * tmp = dest;
	const char * s = src;

	while (len--)
		*tmp++ = *s++;
	return dest;
}

void * memcpy(void * dest, const void * src, size_t len) __attribute__ ((weak, alias ("__memcpy")));
