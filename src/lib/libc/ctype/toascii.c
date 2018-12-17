/*
 * libc/ctype/toascii.c
 */

#include <ctype.h>
#include <xboot/module.h>

int toascii(int c)
{
	return (c & 0x7f);
}
EXPORT_SYMBOL(toascii);
