/*
 * libc/ctype/toascii.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Converts a byte to ASCII
 */
int toascii(int c)
{
	return (c & 0x7f);
}
EXPORT_SYMBOL(toascii);
