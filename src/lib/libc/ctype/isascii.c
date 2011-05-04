/*
 * libc/ctype/isascii.c
 */

#include <ctype.h>

int isascii(int c)
{
	return ((unsigned int)c <= 0x7f);
}
