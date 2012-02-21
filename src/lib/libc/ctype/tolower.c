/*
 * libc/ctype/tolower.c
 */

#include <ctype.h>

/*
 * Converts a byte to lower case
 */
int tolower(int c)
{
	if(isupper(c))
		return ('a' - 'A' + c);
	return c;
}
