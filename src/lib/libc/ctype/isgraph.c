/*
 * libc/ctype/isgraph.c
 */

#include <ctype.h>
#include <xboot/module.h>

int isgraph(int c)
{
	return ((unsigned)c - 0x21) < 0x5e;
}
EXPORT_SYMBOL(isgraph);
