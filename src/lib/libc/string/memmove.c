/*
 * libc/string/memmove.c
 */

#include <types.h>
#include <string.h>

static void * __memmove(void * dest, const void * src, size_t n)
{
	char * tmp;
	const char * s;

	if (dest <= src)
	{
		tmp = dest;
		s = src;
		while (n--)
			*tmp++ = *s++;
	}
	else
	{
		tmp = dest;
		tmp += n;
		s = src;
		s += n;
		while (n--)
			*--tmp = *--s;
	}
	return dest;
}

void * memmove(void * dest, const void * src, size_t n) __attribute__ ((weak, alias ("__memmove")));
