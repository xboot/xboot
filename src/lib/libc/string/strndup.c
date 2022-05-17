/*
 * libc/string/strndup.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <xboot/module.h>

/*
 * Allocates and duplicates of a fixed-size string
 */
char * strndup(const char * s, size_t n)
{
	size_t l = strnlen(s, n);
	char * d = malloc(l + 1);

	if(!d)
		return NULL;
	memcpy(d, s, l);
	d[l] = 0;
	return d;
}
EXPORT_SYMBOL(strndup);
