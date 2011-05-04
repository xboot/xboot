/*
 * libc/ctype/toupper.c
 */

#include <ctype.h>

int toupper(int c)
{
	if(islower(c))
		return ('A' - 'a' + c);
	return c;
}
