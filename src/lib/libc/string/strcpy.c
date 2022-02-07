/*
 * libc/string/strcpy.c
 */

#include <types.h>
#include <string.h>
#include <xboot/module.h>

char * __strcpy(char * dest, const char * src)
{
	char * tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}

/*
 * Copies one string to another
 */
extern __typeof(__strcpy) strcpy __attribute__((weak, alias("__strcpy")));
EXPORT_SYMBOL(strcpy);