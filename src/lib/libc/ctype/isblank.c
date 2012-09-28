/*
 * libc/ctype/isblank.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a blank byte
 */
int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
EXPORT_SYMBOL(isblank);
