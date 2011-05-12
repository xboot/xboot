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

int strncmp(const char * s1, const char * s2, size_t n) __attribute__ ((weak, alias ("__strncmp")));
