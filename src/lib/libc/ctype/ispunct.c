/*
 * libc/ctype/ispunct.c
 */

#include <ctype.h>

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}
EXPORT_SYMBOL(ispunct);
