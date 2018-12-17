/*
 * libc/ctype/islower.c
 */

#include <ctype.h>
#include <xboot/module.h>

int islower(int c)
{
	return ((unsigned)c - 'a') < 26;
}
EXPORT_SYMBOL(islower);
