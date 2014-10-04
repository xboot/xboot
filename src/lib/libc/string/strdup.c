/*
 * libc/string/strdup.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>

/*
 * Allocates and duplicates a string
 */
char * strdup(const char * s)
{
	char * p;

	if(!s)
		return NULL;

	p = malloc(strlen(s) + 1);
	if(p)
		return(strcpy(p, s));

	return NULL;
}
EXPORT_SYMBOL(strdup);
