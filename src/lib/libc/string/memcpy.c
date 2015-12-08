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

/*
 * Copies one buffer to another
 */
extern __typeof(__memcpy) memcpy __attribute__((weak, alias("__memcpy")));
EXPORT_SYMBOL(memcpy);
