/*
 * libc/ctype/isascii.c
 */

#include <ctype.h>

int isascii(int c)
{
	return !(c & ~0x7f);
}
EXPORT_SYMBOL(isascii);
