/*
 * libc/ctype/isupper.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isupper(int c)
{
	return ((unsigned)c - 'A') < 26;
}
EXPORT_SYMBOL(isupper);
