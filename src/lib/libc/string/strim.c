/*
 * libc/string/strim.c
 */

#include <types.h>
#include <ctype.h>
#include <string.h>
#include <xboot/module.h>

/*
 * Removes leading and trailing whitespace
 */
char * strim(char * s)
{
	char * e;

	if(s)
	{
		while(isspace(*s))
			s++;
		if(*s == 0)
			return s;
		e = s + strlen(s) - 1;
		while((e > s) && isspace(*e))
			e--;
		*(e + 1) = 0;
	}
	return s;
}
EXPORT_SYMBOL(strim);
