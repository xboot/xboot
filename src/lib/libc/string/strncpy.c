/*
 * libc/string/strncpy.c
 */

#include <types.h>
#include <string.h>

char * strncpy(char * dest, const char * src, size_t n)
{
	char * tmp = dest;

	while (n)
	{
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		n--;
	}
	return dest;
}
