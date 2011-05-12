/*
 * libc/string/strlcpy.c
 */

#include <types.h>
#include <string.h>

size_t strlcpy(char * dest, const char * src, size_t n)
{
	size_t len;
	size_t ret = strlen(src);

	if (n)
	{
		len = (ret >= n) ? n - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}
