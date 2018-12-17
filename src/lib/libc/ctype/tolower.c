/*
 * libc/ctype/tolower.c
 */

#include <ctype.h>
#include <xboot/module.h>

int tolower(int c)
{
	if(isupper(c))
		return c | 32;
	return c;
}
EXPORT_SYMBOL(tolower);
