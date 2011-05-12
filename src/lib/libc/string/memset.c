/*
 * libc/string/memset.c
 */

#include <types.h>
#include <string.h>

static void * __memset(void * s, int c, size_t n)
{
	char * xs = s;

	while (n--)
		*xs++ = c;

	return s;
}

void * memset(void * s, int c, size_t n) __attribute__ ((weak, alias ("__memset")));
