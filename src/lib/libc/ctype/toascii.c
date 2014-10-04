/*
 * libc/ctype/toascii.c
 */

#include <ctype.h>

int toascii(int c)
{
	return (c & 0x7f);
}
EXPORT_SYMBOL(toascii);
