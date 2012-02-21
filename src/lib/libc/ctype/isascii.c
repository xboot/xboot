/*
 * libc/ctype/isascii.c
 */

#include <ctype.h>

/*
 * Checks if a byte is ASCII
 */
int isascii(int c)
{
	return ((unsigned int)c <= 0x7f);
}
