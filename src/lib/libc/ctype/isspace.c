/*
 * libc/ctype/isspace.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isspace(int c)
{
	return (c == ' ') || ((unsigned)c - '\t' < 5);
}
EXPORT_SYMBOL(isspace);
