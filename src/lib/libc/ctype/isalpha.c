/*
 * libc/ctype/isalpha.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isalpha(int c)
{
	return (((unsigned)c | 32) - 'a') < 26;
}
EXPORT_SYMBOL(isalpha);
