/*
 * libc/string/strcpy.c
 */

#include <types.h>
#include <string.h>

/*
 * Copies one string to another
 */
char * strcpy(char * dest, const char * src)
{
	char * tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}
EXPORT_SYMBOL(strcpy);
