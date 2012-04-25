/*
 * libc/ctype/isblank.c
 */

#include <ctype.h>

/*
 * Checks if a byte is a blank byte
 */
int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
