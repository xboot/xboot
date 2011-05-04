/*
 * libc/ctype/tolower.c
 */

#include <ctype.h>

int tolower(int c)
{
	if(isupper(c))
		return ('a' - 'A' + c);
	return c;
}
