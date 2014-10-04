/*
 * libc/string/strnicmp.c
 */

#include <types.h>
#include <ctype.h>
#include <string.h>

/*
 * Compare strings without case sensitivity
 */
int strnicmp(const char * s1, const char * s2, size_t n)
{
	unsigned char c1 = 0, c2 = 0;

	if (n)
	{
		do {
			c1 = *s1;
			c2 = *s2;
			s1++;
			s2++;
			if (!c1)
				break;
			if (!c2)
				break;
			if (c1 == c2)
				continue;
			c1 = tolower(c1);
			c2 = tolower(c2);
			if (c1 != c2)
				break;
		} while (--n);
	}
	return (int)c1 - (int)c2;
}
EXPORT_SYMBOL(strnicmp);
