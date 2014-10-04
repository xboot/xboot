/*
 * libc/ctype/isblank.c
 */

#include <ctype.h>

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
EXPORT_SYMBOL(isblank);
