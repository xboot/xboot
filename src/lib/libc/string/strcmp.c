/*
 * libc/string/strcmp.c
 */

#include <types.h>
#include <string.h>

static int __strcmp(const char * s1, const char * s2)
{
	int res;

	while (1)
	{
		if ((res = *s1 - *s2++) != 0 || !*s1++)
			break;
	}
	return res;
}

int strcmp(const char * s1, const char * s2) __attribute__ ((weak, alias ("__strcmp")));
