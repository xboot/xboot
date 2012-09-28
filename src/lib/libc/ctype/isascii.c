/*
 * libc/ctype/isascii.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is ASCII
 */
int isascii(int c)
{
	return ((unsigned int)c <= 0x7f);
}
EXPORT_SYMBOL(isascii);
