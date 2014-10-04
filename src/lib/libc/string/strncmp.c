/*
 * libc/string/strncmp.c
 */

#include <types.h>
#include <string.h>

static int __strncmp(const char * s1, const char * s2, size_t n)
{
	int __res = 0;

	while (n)
	{
		if ((__res = *s1 - *s2++) != 0 || !*s1++)
			break;
		n--;
	}
	return __res;
}

/*
 * Compares a specific number of bytes in two strings
 */
extern __typeof(__strncmp) strncmp __attribute__((weak, alias("__strncmp")));
EXPORT_SYMBOL(strncmp);
