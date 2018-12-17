/*
 * libc/ctype/isblank.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
EXPORT_SYMBOL(isblank);
