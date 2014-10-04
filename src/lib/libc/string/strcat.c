/*
 * libc/string/strcat.c
 */

#include <string.h>

/*
 * Appends one string to another
 */
char * strcat(char * dest, const char * src)
{
	char * tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');

	return tmp;
}
EXPORT_SYMBOL(strcat);
