/*
 * libc/string/strcpy.c
 */

#include <types.h>
#include <string.h>

char * strcpy(char * dest, const char * src)
{
	char * tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}
