/*
 * libc/string/memchr.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

void * memchr(const void * s, int c, size_t n)
{
	const unsigned char *p = s;

	while (n-- != 0)
	{
        if ((unsigned char)c == *p++)
        {
			return (void *)(p - 1);
		}
	}

	return NULL;
}
