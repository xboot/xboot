/*
 * libc/ctype/ispunct.c
 */

#include <ctype.h>
#include <xboot/module.h>

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}
EXPORT_SYMBOL(ispunct);
