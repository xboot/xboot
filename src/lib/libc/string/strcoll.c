/*
 * libc/string/strcpy.c
 */

#include <types.h>
#include <string.h>

/*
 * Compares two strings according to the current locale
 */
int strcoll(const char * s1, const char * s2)
{
	return strcmp(s1, s2);
}
EXPORT_SYMBOL(strcoll);
