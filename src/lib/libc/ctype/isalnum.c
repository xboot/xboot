/*
 * libc/ctype/isalnum.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}
EXPORT_SYMBOL(isalnum);
