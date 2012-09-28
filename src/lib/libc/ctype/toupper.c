/*
 * libc/ctype/toupper.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Converts a byte to upper case
 */
int toupper(int c)
{
	if(islower(c))
		return ('A' - 'a' + c);
	return c;
}
EXPORT_SYMBOL(toupper);
