/*
 * libc/ctype/isprint.c
 */

#include <ctype.h>

int isprint(int c)
{
	return ((unsigned)c - 0x20) < 0x5f;
}
EXPORT_SYMBOL(isprint);
