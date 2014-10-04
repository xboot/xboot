/*
 * libc/string/strncasecmp.c
 */

#include <types.h>
#include <ctype.h>
#include <string.h>

/*
 * Compares the two strings s1 and s2, ignoring the case of the characters
 */
int strncasecmp(const char * s1, const char * s2, size_t n)
{
	int c1, c2;

	do {
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
	} while ((--n > 0) && c1 == c2 && c1 != 0);

	return c1 - c2;
}
EXPORT_SYMBOL(strncasecmp);
